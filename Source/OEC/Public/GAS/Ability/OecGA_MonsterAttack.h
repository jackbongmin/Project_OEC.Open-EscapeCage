// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/OecGameplayAbility.h"
#include "OecGA_MonsterAttack.generated.h"

/**
 * 
 */

class UGameplayEffect;

UCLASS()
class OEC_API UOecGA_MonsterAttack : public UOecGameplayAbility
{
	GENERATED_BODY()
	

public:
	UOecGA_MonsterAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnEventReceived(FGameplayEventData InPayload);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

};
