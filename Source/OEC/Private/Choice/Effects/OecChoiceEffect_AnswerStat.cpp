// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_AnswerStat.h"
#include "Choice/OecChoiceSubsystem.h"

int32 UOecChoiceEffect_AnswerStat::GetDuration(const FOecChoiceEffect& InEffect) const
{
	const int32 duration = FCString::Atoi(*InEffect.Param.ToString());
	return duration > 0 ? duration : DefaultDuration;
}

void UOecChoiceEffect_AnswerStat::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	if (!InContext.ChoiceSubsystem) return;

	InContext.ChoiceSubsystem->AddAnswerModifier(EffectTag, InEffect.Value, GetDuration(InEffect), true);
}

void UOecChoiceEffect_AnswerStat::OnAnswerTick(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const
{
	if (!InContext.ChoiceSubsystem) return;

	FOecChoiceEffect statEffect;
	statEffect.EffectTag = StatEffectTag;
	statEffect.Value = InModifier.Value;

	InContext.ChoiceSubsystem->ExecuteEffect(statEffect, InContext.Instigator);
}

FString UOecChoiceEffect_AnswerStat::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return FString::Printf(TEXT("답변 %d번 동안 매번 %s %+g"), GetDuration(InEffect), *StatName, InEffect.Value);
}
