// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Filters/OecQuestionFilter.h"
#include "OecQuestionFilter_RunCount.generated.h"

/**
 * 최소 회차: MinRunCount 회차부터 출제 (반복을 다루는 스토리 갈래용)
 */
UCLASS()
class OEC_API UOecQuestionFilter_RunCount : public UOecQuestionFilter
{
	GENERATED_BODY()

public:
	virtual bool PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const override;
};
