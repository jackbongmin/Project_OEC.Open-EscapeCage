// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Darken.h"
#include "Choice/OecChoiceSubsystem.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_Darken::UOecChoiceEffect_Darken()
{
	EffectTag = OecGameplayTags::Effect_Env_Darken;
}

void UOecChoiceEffect_Darken::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	if (!InContext.ChoiceSubsystem) return;

	// 다시 걸리면 남은 답변 수를 갱신 (누적 X)
	const int32 duration = FMath::RoundToInt(InEffect.Value);
	InContext.ChoiceSubsystem->AddAnswerModifier(EffectTag, 1.0f, duration > 0 ? duration : INDEX_NONE, false);
}

FString UOecChoiceEffect_Darken::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	const int32 duration = FMath::RoundToInt(InEffect.Value);
	return duration > 0 ? FString::Printf(TEXT("방이 어두워짐 (답변 %d번 동안)"), duration) : TEXT("방이 어두워짐");
}
