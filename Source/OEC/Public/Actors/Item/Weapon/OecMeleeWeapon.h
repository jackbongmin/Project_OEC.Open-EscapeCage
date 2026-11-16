// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Item/Weapon/OecWeaponBase.h"
#include "OecMeleeWeapon.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class USoundBase;
class UCameraShakeBase;

/**
 * 근접 무기
 *
 * - 좌클릭 → 무기 어빌리티(UOecGA_MeleeAttack) 발동. 휘두르기·타격 판정·피해는 어빌리티가 처리한다.
 * - 타격 판정은 무기 콜리전이 아니라 카메라 앞 구체 스윕으로 한다 (1인칭 팔 애니메이션에 판정이 흔들리지 않게).
 * - 공격 몽타주에 애님 노티파이 SendEvent(Event.Weapon.MeleeHit)를 넣은 순간 판정한다.
 * - 이 액터는 수치와 연출(소리, 이펙트, 카메라 흔들림)만 가진다.
 */
UCLASS()
class OEC_API AOecMeleeWeapon : public AOecWeaponBase
{
	GENERATED_BODY()

public:
	AOecMeleeWeapon();

	virtual void InitWeaponData(const FItemStaticData& InWeaponData) override;

	virtual void StartAttack() override;
	virtual void StopAttack() override;

	// 휘두를 때 연출 (소리)
	void PlaySwingFX();

	// 맞았을 때 연출 (이펙트, 소리, 카메라 흔들림)
	void PlayHitFX(const FHitResult& InHitResult);

protected:
	// 무기 어빌리티 입력(Fire) 전달
	void TriggerAttackAbility();

protected:
	// 스태틱 메시 무기용 (도끼, 파이프 등). 스켈레탈 메시가 없으면 여기에 메시를 넣는다.
	// 손 소켓(HandGrip_R) 기준 위치·회전은 이 컴포넌트의 트랜스폼으로 무기마다 맞춘다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> StaticWeaponMesh;

	/* ==========================================
	   근접 공격 수치 (데이터 테이블 AttackRange가 0보다 크면 덮어씀)
	========================================== */
	// 카메라에서 앞으로 닿는 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Melee")
	float AttackRange = 200.0f;

	// 스윕 구체 반경 (클수록 대충 휘둘러도 맞음)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Melee", meta = (ClampMin = "1.0"))
	float SweepRadius = 35.0f;

	// 한 번 휘두를 때 최대 몇 명까지 맞는지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Melee", meta = (ClampMin = "1"))
	int32 MaxHitCount = 3;

	// 몽타주가 없을 때 다음 공격까지 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Melee", meta = (ClampMin = "0.05"))
	float AttackIntervalWithoutMontage = 0.6f;

	// 공격 몽타주 (데이터 테이블 FireMontage)
	UPROPERTY(BlueprintReadOnly, Category = "OEC|Weapon|Melee")
	TObjectPtr<UAnimMontage> AttackMontage;

	/* ==========================================
	   연출
	========================================== */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TObjectPtr<USoundBase> SwingSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TObjectPtr<UNiagaraSystem> HitFX;

	UPROPERTY(EditDefaultsOnly, Category = "OEC|Feedback")
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass;

public:
	float GetAttackRange() const { return AttackRange; }
	float GetSweepRadius() const { return SweepRadius; }
	int32 GetMaxHitCount() const { return MaxHitCount; }
	float GetAttackIntervalWithoutMontage() const { return AttackIntervalWithoutMontage; }
	UAnimMontage* GetAttackMontage() const { return AttackMontage; }
};
