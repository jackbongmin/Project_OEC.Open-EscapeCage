// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffect_AnswerStat.h"
#include "OecChoiceEffect_AnswerHealth.generated.h"

/**
 * Effect.Passive.AnswerHealth: 이후 답변마다 체력 변화 (예: 10회 동안 +2)
 */
UCLASS()
class OEC_API UOecChoiceEffect_AnswerHealth : public UOecChoiceEffect_AnswerStat
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_AnswerHealth();
};
