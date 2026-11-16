// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OecInteractionComponent.generated.h"

class UPrimitiveComponent;
class UOecInteractRangeComponent;
struct FHitResult;

// E 대상이 바뀜 (없으면 nullptr). 크로스헤어 등 UI 갱신용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractTargetChangedDelegate, AActor*, InNewTarget);

/**
 * 플레이어 상호작용 (E키)
 *
 * 1. 후보: 대상의 UOecInteractRangeComponent 범위에 들어가면 후보 목록에 추가, 나오면 제거
 * 2. 선택: 후보가 있을 때만 Tick을 켜고, 화면 중앙에 가장 가까운(시선 각도가 작은) 대상 하나를 고른다. 벽 너머 대상은 제외
 * 3. 실행: E를 누르면 선택된 대상의 Interact 호출
 *
 * - 근처에만 가면 작은 아이템도 쉽게 줍고, 붙어 있는 빨강/파랑 버튼은 바라보는 쪽이 선택된다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OEC_API UOecInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UOecInteractionComponent();

    // E키 입력과 바인딩될 핵심 함수
    UFUNCTION(BlueprintCallable, Category = "OEC|Interaction")
    void TryInteract();

    UFUNCTION(BlueprintPure, Category = "OEC|Interaction")
    AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

    UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
    FOnInteractTargetChangedDelegate OnInteractTargetChanged;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
    void OnOwnerBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult);

    UFUNCTION()
    void OnOwnerEndOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex);

    void AddCandidate(UOecInteractRangeComponent* InRange);
    void RemoveCandidate(UOecInteractRangeComponent* InRange);

    // 후보 중 지금 E를 띄울 대상 고르기
    AActor* FindBestTarget() const;

    void SetCurrentTarget(AActor* InNewTarget);

    // 카메라에서 대상까지 가리는 것이 없는지
    bool IsTargetVisible(AActor* InTarget, const FVector& InViewLocation, const FVector& InTargetLocation) const;

protected:
    // 화면 중앙에서 이 각도(도) 안에 있는 대상만 선택 (클수록 대충 봐도 잡힘)
    UPROPERTY(EditAnywhere, Category = "OEC|Interaction", meta = (ClampMin = "1.0", ClampMax = "90.0"))
    float MaxInteractAngle = 35.0f;

    // 벽 너머 대상 제외 (가시성 채널로 검사)
    UPROPERTY(EditAnywhere, Category = "OEC|Interaction")
    bool bRequireLineOfSight = true;

private:
    // 범위 안에 있는 대상들의 범위 컴포넌트
    TArray<TWeakObjectPtr<UOecInteractRangeComponent>> Candidates;

    TWeakObjectPtr<AActor> CurrentTarget;

    // 플레이어 몸통 콜리전 (오버랩 이벤트 수신용)
    TWeakObjectPtr<UPrimitiveComponent> OwnerCollision;
};
