// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_Turret.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

AOecRoomEvent_Turret::AOecRoomEvent_Turret()
{
	// 총·근접 무기가 맞힐 수 있게: 가시성 채널 막기, 근접 판정(Pawn 오브젝트)에도 걸리게
	TurretBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBody"));
	TurretBody->SetupAttachment(RootComponent);
	TurretBody->SetRelativeScale3D(FVector(1.0f));
	TurretBody->SetCollisionObjectType(ECC_Pawn);
	TurretBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TurretBody->SetCollisionResponseToAllChannels(ECR_Block);

	TurretBarrel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBarrel"));
	TurretBarrel->SetupAttachment(TurretBody);
	TurretBarrel->SetRelativeLocation(FVector(60.0f, 0.0f, 0.0f));
	TurretBarrel->SetRelativeScale3D(FVector(0.8f, 0.15f, 0.15f));
	TurretBarrel->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> sphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (sphereMesh.Succeeded()) TurretBody->SetStaticMesh(sphereMesh.Object);
	if (cubeMesh.Succeeded()) TurretBarrel->SetStaticMesh(cubeMesh.Object);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UOecAttributeSetBase>(TEXT("AttributeSet"));

	DestroyDelay = 1.0f;
}

void AOecRoomEvent_Turret::BeginPlay()
{
	Super::BeginPlay();

	TurretBody->SetRelativeLocation(FVector(0.0f, 0.0f, TurretHeight));

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetMaxHealthAttribute(), TurretHealth);
	AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetHealthAttribute(), TurretHealth);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOecAttributeSetBase::GetHealthAttribute()).AddUObject(this, &AOecRoomEvent_Turret::OnTurretHealthChanged);
}

void AOecRoomEvent_Turret::OnEventStarted()
{
	SetBasicShapeColor(TurretBody, TurretColor);
	SetBasicShapeColor(TurretBarrel, FLinearColor(0.1f, 0.1f, 0.1f));
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AOecRoomEvent_Turret::Fire, FireInterval, true, FireInterval);
}

void AOecRoomEvent_Turret::Fire()
{
	const AActor* player = GetTargetPlayer();
	if (!player || IsEventFinished()) return;

	// 1. 조준
	const FVector bodyLocation = TurretBody->GetComponentLocation();
	const FVector targetLocation = player->GetActorLocation();
	TurretBody->SetWorldRotation((targetLocation - bodyLocation).Rotation());

	// 2. 사격: 엄폐물(벽, 기둥)에 막히면 빗나감
	const FVector muzzleLocation = TurretBarrel->GetComponentLocation();
	FCollisionQueryParams queryParams(SCENE_QUERY_STAT(OecTurretShot), false, this);
	FHitResult hitResult;
	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(hitResult, muzzleLocation, targetLocation, ECC_Visibility, queryParams);
	const bool bHitPlayer = !bBlocked || hitResult.GetActor() == player;
	const FVector endLocation = bBlocked ? hitResult.ImpactPoint : targetLocation;

	if (bHitPlayer)
	{
		ApplyDamageToPlayer(ShotDamage);
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawShotDebug)
	{
		DrawDebugLine(GetWorld(), muzzleLocation, endLocation, bHitPlayer ? FColor::Red : FColor::Yellow, false, 0.15f, 0, 2.0f);
	}
#endif

	BP_OnTurretFired(muzzleLocation, endLocation, bHitPlayer);
}

void AOecRoomEvent_Turret::OnTurretHealthChanged(const FOnAttributeChangeData& InData)
{
	if (InData.NewValue > 0.0f || IsEventFinished()) return;

	UE_LOG(LogTemp, Log, TEXT("[%s] 터렛 파괴"), *GetName());
	CompleteEvent(true);
}

void AOecRoomEvent_Turret::OnEventFinished(bool bInSuccess)
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);

	if (bInSuccess)
	{
		TurretBody->SetVisibility(false, true);
		TurretBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
