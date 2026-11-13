// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/OecGameplayAbility.h"
#include "OecGA_WeaponFire.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecGA_WeaponFire : public UOecGameplayAbility
{
	GENERATED_BODY()
	
public:
	UOecGA_WeaponFire();

protected:
	virtual bool CheckCost(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, OUT FGameplayTagContainer* OutOptionalRelevantTags = nullptr) const override;

	virtual void ApplyCost(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|Damage")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
};
