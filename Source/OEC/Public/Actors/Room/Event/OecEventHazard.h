// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OecEventHazard.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class AOecRoomEventBase;

/**
 * 이벤트 위험물 (튀는 구체, 떨어지는 잔해 등)
 *
 * - 플레이어에게 닿으면 소유 이벤트를 통해 피해를 준다 (HitCooldown 동안 같은 대상 재피격 없음).
 * - 움직임은 ProjectileMovement: 튕김/중력/속도를 스폰한 이벤트가 정한다.
 * - 고정 지형(WorldStatic)에만 막히고, 다른 위험물·픽업·몬스터는 통과한다.
 */
UCLASS()
class OEC_API AOecEventHazard : public AActor
{
	GENERATED_BODY()

public:
	AOecEventHazard();

	// 스폰 직후 호출: 소유 이벤트, 피해량, 움직임 설정
	void InitHazard(AOecRoomEventBase* InOwnerEvent, float InDamage, const FVector& InVelocity, bool bInBounce, float InGravityScale, bool bInDestroyOnStop);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHazardBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult);

	UFUNCTION()
	void OnProjectileStop(const FHitResult& InImpactResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UProjectileMovementComponent> Movement;

	// 기본 도형 메시 색 (블루프린트에서 메시를 바꾸면 쓰지 않음)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Hazard")
	FLinearColor HazardColor = FLinearColor(0.2f, 0.8f, 1.0f);

	// 같은 대상에게 다시 피해를 주기까지의 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Hazard", meta = (ClampMin = "0"))
	float HitCooldown = 1.0f;

private:
	TWeakObjectPtr<AOecRoomEventBase> OwnerEvent;

	float Damage = 0.0f;

	bool bDestroyOnStop = false;

	float LastHitTime = -1000.0f;
};
