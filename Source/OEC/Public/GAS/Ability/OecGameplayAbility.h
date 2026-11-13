// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/OecEnumType.h"
#include "OecGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UOecGameplayAbility();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Ability")
	EOecAbilityInputID AbilityInputID = EOecAbilityInputID::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Ability")
	bool bActivateOnGranted = false;

protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilitySpec& InSpec) override;
};
