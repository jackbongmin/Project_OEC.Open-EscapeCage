// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_Buff.generated.h"

/**
 * Effect.Buff: 버프 GE 적용. Param = 버프 ID (프로젝트 설정의 ChoiceBuffEffects), Value = GE 레벨
 */
UCLASS()
class OEC_API UOecChoiceEffect_Buff : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Buff();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
};
