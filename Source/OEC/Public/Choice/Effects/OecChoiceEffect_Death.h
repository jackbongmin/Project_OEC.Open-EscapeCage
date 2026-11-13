// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_Death.generated.h"

/**
 * Effect.Death: 즉시 사망 (체력 GE로 현재 체력만큼 감소)
 * 확률 즉사는 결과 가중치로 표현한다. 예) 7: [Effect.Death] / 93: [Effect.None]
 */
UCLASS()
class OEC_API UOecChoiceEffect_Death : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Death();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
};
