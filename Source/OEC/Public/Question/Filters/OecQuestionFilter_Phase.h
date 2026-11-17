// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Filters/OecQuestionFilter.h"
#include "OecQuestionFilter_Phase.generated.h"

/**
 * 페이즈 제한: AllowedPhases가 비어 있으면 모든 페이즈에서 출제
 */
UCLASS()
class OEC_API UOecQuestionFilter_Phase : public UOecQuestionFilter
{
	GENERATED_BODY()

public:
	virtual bool PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const override;
};
