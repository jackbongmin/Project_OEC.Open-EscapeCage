// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_Stack.generated.h"

/**
 * Effect.Stack: 스택 증감. Value = 증감량
 * 페이즈 1이면 전구 스택(StageManager), 페이즈 2면 문 열쇠 스택(EventSubsystem)으로 들어간다.
 */
UCLASS()
class OEC_API UOecChoiceEffect_Stack : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Stack();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
};
