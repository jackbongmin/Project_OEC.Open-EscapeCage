// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGameplayAbility.h"
#include "AbilitySystemComponent.h"

UOecGameplayAbility::UOecGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOecGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilitySpec& InSpec)
{
	Super::OnAvatarSet(InActorInfo, InSpec);

	if (bActivateOnGranted)
	{
		InActorInfo->AbilitySystemComponent->TryActivateAbility(InSpec.Handle, false);
	}
}