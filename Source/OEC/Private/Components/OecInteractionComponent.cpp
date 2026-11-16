// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OecInteractionComponent.h"
#include "Components/OecInteractRangeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OecInteractableInterface.h"

UOecInteractionComponent::UOecInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    // 후보가 생길 때만 켬
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UOecInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* owner = GetOwner();
    if (!owner) return;

    // 캐릭터면 캡슐(루트)이 오버랩을 받음
    UPrimitiveComponent* ownerCollision = Cast<UPrimitiveComponent>(owner->GetRootComponent());
    if (!ownerCollision)
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] 루트가 콜리전 컴포넌트가 아니라 상호작용 범위를 감지할 수 없습니다"), *owner->GetName());
        return;
    }

    OwnerCollision = ownerCollision;
    ownerCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &UOecInteractionComponent::OnOwnerBeginOverlap);
    ownerCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &UOecInteractionComponent::OnOwnerEndOverlap);

    // 시작할 때 이미 범위 안에 있던 대상 등록
    TArray<UPrimitiveComponent*> overlappingComponents;
    ownerCollision->GetOverlappingComponents(overlappingComponents);
    for (UPrimitiveComponent* component : overlappingComponents)
    {
        AddCandidate(Cast<UOecInteractRangeComponent>(component));
    }
}

void UOecInteractionComponent::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
    if (UPrimitiveComponent* ownerCollision = OwnerCollision.Get())
    {
        ownerCollision->OnComponentBeginOverlap.RemoveDynamic(this, &UOecInteractionComponent::OnOwnerBeginOverlap);
        ownerCollision->OnComponentEndOverlap.RemoveDynamic(this, &UOecInteractionComponent::OnOwnerEndOverlap);
    }

    Super::EndPlay(InEndPlayReason);
}

void UOecInteractionComponent::OnOwnerBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult)
{
    AddCandidate(Cast<UOecInteractRangeComponent>(InOtherComp));
}

void UOecInteractionComponent::OnOwnerEndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex)
{
    RemoveCandidate(Cast<UOecInteractRangeComponent>(InOtherComp));
}

void UOecInteractionComponent::AddCandidate(UOecInteractRangeComponent* InRange)
{
    if (!InRange || !InRange->GetOwner()) return;
    if (!InRange->GetOwner()->Implements<UOecInteractableInterface>()) return;

    Candidates.AddUnique(InRange);
    SetComponentTickEnabled(true);
}

void UOecInteractionComponent::RemoveCandidate(UOecInteractRangeComponent* InRange)
{
    if (!InRange) return;

    Candidates.Remove(InRange);

    if (CurrentTarget.Get() == InRange->GetOwner())
    {
        SetCurrentTarget(nullptr);
    }

    if (Candidates.Num() == 0)
    {
        SetComponentTickEnabled(false);
    }
}

void UOecInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 파괴된 대상(주운 아이템 등) 정리
    Candidates.RemoveAll([](const TWeakObjectPtr<UOecInteractRangeComponent>& InRange) { return !InRange.IsValid(); });

    SetCurrentTarget(FindBestTarget());

    if (Candidates.Num() == 0)
    {
        SetComponentTickEnabled(false);
    }
}

AActor* UOecInteractionComponent::FindBestTarget() const
{
    const APawn* ownerPawn = Cast<APawn>(GetOwner());
    const APlayerController* pc = ownerPawn ? Cast<APlayerController>(ownerPawn->GetController()) : nullptr;
    if (!pc) return nullptr;

    FVector viewLocation;
    FRotator viewRotation;
    pc->GetPlayerViewPoint(viewLocation, viewRotation);
    const FVector viewDirection = viewRotation.Vector();

    const float minDot = FMath::Cos(FMath::DegreesToRadians(MaxInteractAngle));

    // 1. 시선 각도 안에 있는 후보를 화면 중앙에 가까운 순으로 정렬
    TArray<TPair<float, AActor*>> sortedTargets;
    for (const TWeakObjectPtr<UOecInteractRangeComponent>& range : Candidates)
    {
        AActor* target = range.IsValid() ? range->GetOwner() : nullptr;
        if (!target) continue;
        if (!IOecInteractableInterface::Execute_CanInteract(target, GetOwner())) continue;

        const FVector targetLocation = IOecInteractableInterface::Execute_GetInteractLocation(target);
        const float dot = FVector::DotProduct(viewDirection, (targetLocation - viewLocation).GetSafeNormal());
        if (dot < minDot) continue;

        sortedTargets.Add(TPair<float, AActor*>(dot, target));
    }

    sortedTargets.Sort([](const TPair<float, AActor*>& InA, const TPair<float, AActor*>& InB) { return InA.Key > InB.Key; });

    // 2. 가려지지 않은 첫 번째 대상
    for (const TPair<float, AActor*>& pair : sortedTargets)
    {
        if (!bRequireLineOfSight || IsTargetVisible(pair.Value, viewLocation, IOecInteractableInterface::Execute_GetInteractLocation(pair.Value)))
        {
            return pair.Value;
        }
    }

    return nullptr;
}

bool UOecInteractionComponent::IsTargetVisible(AActor* InTarget, const FVector& InViewLocation, const FVector& InTargetLocation) const
{
    AActor* owner = GetOwner();

    FCollisionQueryParams collisionParams(SCENE_QUERY_STAT(OecInteractSight), false, owner);

    // 손에 든 무기 등 플레이어에 붙은 액터도 무시
    TArray<AActor*> attachedActors;
    owner->GetAttachedActors(attachedActors, true, true);
    collisionParams.AddIgnoredActors(attachedActors);

    FHitResult hitResult;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, InViewLocation, InTargetLocation, ECC_Visibility, collisionParams);

    // 아무것도 안 맞았거나 대상 자신이 맞았으면 보이는 것
    return !bHit || hitResult.GetActor() == InTarget;
}

void UOecInteractionComponent::SetCurrentTarget(AActor* InNewTarget)
{
    // 대상이 파괴된 경우(IsStale)에는 같은 nullptr이어도 UI에 알려야 함
    if (CurrentTarget.Get() == InNewTarget && !CurrentTarget.IsStale()) return;

    if (AActor* oldTarget = CurrentTarget.Get())
    {
        IOecInteractableInterface::Execute_SetInteractFocused(oldTarget, false);
    }

    CurrentTarget = InNewTarget;

    if (InNewTarget)
    {
        IOecInteractableInterface::Execute_SetInteractFocused(InNewTarget, true);
    }

    OnInteractTargetChanged.Broadcast(InNewTarget);
}

void UOecInteractionComponent::TryInteract()
{
    AActor* target = CurrentTarget.Get();
    if (!target) return;

    AActor* owner = GetOwner();
    if (!IOecInteractableInterface::Execute_CanInteract(target, owner)) return;

    IOecInteractableInterface::Execute_Interact(target, owner);
    UE_LOG(LogTemp, Log, TEXT("상호작용 성공: %s"), *target->GetName());

    // 상호작용 후 상태가 바뀌었을 수 있으니(문 열림, 아이템 획득) 다음 Tick에서 다시 고름
    if (!IsValid(target) || !IOecInteractableInterface::Execute_CanInteract(target, owner))
    {
        SetCurrentTarget(nullptr);
    }
}
