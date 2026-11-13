// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_None.generated.h"

/**
 * Effect.None: 아무 일도 일어나지 않음 (무의미 질문, 확률 결과의 "꽝")
 */
UCLASS()
class OEC_API UOecChoiceEffect_None : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_None();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
	virtual bool IsEmptyEffect() const override { return true; }
};
