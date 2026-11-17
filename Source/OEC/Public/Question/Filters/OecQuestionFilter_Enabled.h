// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Filters/OecQuestionFilter.h"
#include "OecQuestionFilter_Enabled.generated.h"

/**
 * 꺼 둔 질문 제외 (아직 없는 콘텐츠를 쓰는 질문)
 */
UCLASS()
class OEC_API UOecQuestionFilter_Enabled : public UOecQuestionFilter
{
	GENERATED_BODY()

public:
	virtual bool PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const override;
};
