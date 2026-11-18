// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffect_AnswerStat.h"
#include "OecChoiceEffect_AnswerSanity.generated.h"

/**
 * Effect.Passive.AnswerSanity: 이후 답변마다 정신력 변화 (답변 기본 감소와 별개로 더해짐)
 */
UCLASS()
class OEC_API UOecChoiceEffect_AnswerSanity : public UOecChoiceEffect_AnswerStat
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_AnswerSanity();
};
