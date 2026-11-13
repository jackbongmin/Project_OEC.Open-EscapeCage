// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "Choice/OecChoiceSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"

FString UOecChoiceEffectExecutor::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return FString::Printf(TEXT("%s %g"), *InEffect.EffectTag.ToString(), InEffect.Value);
}

UAbilitySystemComponent* UOecChoiceEffectExecutor::GetInstigatorASC(const FOecChoiceEffectContext& InContext) const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InContext.Instigator);
}

UWorld* UOecChoiceEffectExecutor::GetContextWorld(const FOecChoiceEffectContext& InContext) const
{
	if (InContext.Instigator)
	{
		return InContext.Instigator->GetWorld();
	}

	return InContext.ChoiceSubsystem ? InContext.ChoiceSubsystem->GetWorld() : nullptr;
}

UGameInstance* UOecChoiceEffectExecutor::GetOwningGameInstance() const
{
	const UOecChoiceSubsystem* choiceSubsystem = Cast<UOecChoiceSubsystem>(GetOuter());
	return choiceSubsystem ? choiceSubsystem->GetGameInstance() : nullptr;
}

bool UOecChoiceEffectExecutor::ApplyEffectToInstigator(const FOecChoiceEffectContext& InContext, TSubclassOf<UGameplayEffect> InEffectClass, float InSetByCallerValue, float InLevel) const
{
	UAbilitySystemComponent* asc = GetInstigatorASC(InContext);
	if (!asc || !InEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ASC 또는 GE가 없어 효과를 적용하지 못했습니다"), *EffectTag.ToString());
		return false;
	}

	FGameplayEffectContextHandle contextHandle = asc->MakeEffectContext();
	contextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle specHandle = asc->MakeOutgoingSpec(InEffectClass, InLevel, contextHandle);
	if (!specHandle.IsValid()) return false;

	specHandle.Data.Get()->SetSetByCallerMagnitude(EffectTag, InSetByCallerValue);
	asc->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());

	return true;
}
