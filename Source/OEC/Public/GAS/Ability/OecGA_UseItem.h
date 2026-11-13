// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/OecGameplayAbility.h"
#include "Engine/DataTable.h"
#include "OecGA_UseItem.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecGA_UseItem : public UOecGameplayAbility
{
	GENERATED_BODY()

public:
	UOecGA_UseItem();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "OEC|Item")
	FDataTableRowHandle ItemDataRow;

	UPROPERTY(EditDefaultsOnly, Category = "OEC|Effect")
	TSubclassOf<class UGameplayEffect> ItemEffectClass;
	
};
