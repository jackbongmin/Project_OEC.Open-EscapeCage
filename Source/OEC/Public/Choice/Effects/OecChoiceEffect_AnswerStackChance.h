// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_AnswerStackChance.generated.h"

/**
 * Effect.Passive.AnswerStackChance: 이후 답변할 때마다 확률로 스택 +1. Value = 확률 (0.01 = 1%)
 * 같은 효과를 다시 얻으면 확률이 누적된다.
 */
UCLASS()
class OEC_API UOecChoiceEffect_AnswerStackChance : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_AnswerStackChance();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
	virtual void OnAnswerTick(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const override;
};
