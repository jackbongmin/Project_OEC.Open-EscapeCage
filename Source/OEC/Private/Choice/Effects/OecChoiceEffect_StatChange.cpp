// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_StatChange.h"
#include "GameplayEffect.h"

void UOecChoiceEffect_StatChange::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	TSubclassOf<UGameplayEffect> effectClass = GetStatEffectClass().LoadSynchronous();
	if (!effectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] 프로젝트 설정(OEC Game Data Settings)에 GE가 등록되지 않았습니다"), *EffectTag.ToString());
		return;
	}

	if (ApplyEffectToInstigator(InContext, effectClass, InEffect.Value))
	{
		UE_LOG(LogTemp, Log, TEXT("%s %+g"), *StatDisplayName, InEffect.Value);
	}
}

FString UOecChoiceEffect_StatChange::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	const int32 amount = FMath::Abs(FMath::RoundToInt(InEffect.Value));
	return FString::Printf(TEXT("%s %d %s"), *StatDisplayName, amount, InEffect.Value >= 0.0f ? TEXT("회복") : TEXT("감소"));
}
