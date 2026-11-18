// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Charge.h"
#include "Choice/OecChoiceSubsystem.h"

void UOecChoiceEffect_Charge::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	if (!InContext.ChoiceSubsystem) return;

	const float chargeCount = FMath::Max(FMath::RoundToFloat(InEffect.Value), 1.0f);
	InContext.ChoiceSubsystem->AddAnswerModifier(EffectTag, chargeCount, INDEX_NONE, true);
}

FString UOecChoiceEffect_Charge::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return ChargeDescription;
}
