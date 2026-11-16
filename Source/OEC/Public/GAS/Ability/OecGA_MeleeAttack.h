// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/OecGameplayAbility.h"
#include "OecGA_MeleeAttack.generated.h"

class UGameplayEffect;
class AOecMeleeWeapon;

/**
 * 근접 무기 공격
 *
 * 1. 공격 몽타주 재생 (데이터 테이블 FireMontage)
 * 2. 몽타주의 애님 노티파이 SendEvent(Event.Weapon.MeleeHit) 순간 카메라 앞으로 구체 스윕 → 맞은 대상에 피해 GE
 * 3. 몽타주가 끝나면 종료 (끝나기 전에는 다시 휘두를 수 없음)
 * - 몽타주가 없으면 바로 판정하고, 무기의 AttackIntervalWithoutMontage 뒤에 종료한다 (애니메이션 없이 테스트용).
 */
UCLASS()
class OEC_API UOecGA_MeleeAttack : public UOecGameplayAbility
{
	GENERATED_BODY()

public:
	UOecGA_MeleeAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled) override;

protected:
	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData InPayload);

	UFUNCTION()
	void OnAttackIntervalFinished();

	// 카메라 앞 스윕 → 피해 적용
	void PerformHit();

	void ApplyDamageTo(AActor* InTarget, const FHitResult& InHitResult);

	AOecMeleeWeapon* GetMeleeWeapon() const;

protected:
	// 피해 GE (SetByCaller Data.Damage)
	UPROPERTY(EditDefaultsOnly, Category = "OEC|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 스윕 범위 디버그 표시
	UPROPERTY(EditDefaultsOnly, Category = "OEC|Debug")
	bool bDrawDebugSweep = false;

private:
	// 이번 휘두르기에서 판정을 했는지 (노티파이 누락 경고용, 한 번만 판정)
	bool bHitPerformed = false;
};
