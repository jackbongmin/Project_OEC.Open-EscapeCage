// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_AnswerStackChance.h"
#include "Choice/OecChoiceSubsystem.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_AnswerStackChance::UOecChoiceEffect_AnswerStackChance()
{
	EffectTag = OecGameplayTags::Effect_Passive_AnswerStackChance;
}

void UOecChoiceEffect_AnswerStackChance::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	if (!InContext.ChoiceSubsystem) return;

	// 영구 지속, 다시 얻으면 확률 누적
	InContext.ChoiceSubsystem->AddAnswerModifier(EffectTag, InEffect.Value, INDEX_NONE, true);
}

void UOecChoiceEffect_AnswerStackChance::OnAnswerTick(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const
{
	if (!InContext.ChoiceSubsystem) return;
	if (FMath::FRand() >= InModifier.Value) return;

	UE_LOG(LogTemp, Log, TEXT("지속 효과 발동: %.0f%% 확률로 스택 +1"), InModifier.Value * 100.0f);

	FOecChoiceEffect stackEffect;
	stackEffect.EffectTag = OecGameplayTags::Effect_Stack;
	stackEffect.Value = 1.0f;

	InContext.ChoiceSubsystem->ExecuteEffect(stackEffect, InContext.Instigator);
}

FString UOecChoiceEffect_AnswerStackChance::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return FString::Printf(TEXT("앞으로 답변할 때마다 %g%% 확률로 스택 +1"), InEffect.Value * 100.0f);
}
