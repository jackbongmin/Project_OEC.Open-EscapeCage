// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Filters/OecQuestionFilter.h"
#include "OecQuestionFilter_Prerequisite.generated.h"

/**
 * 선행 조건 (연쇄 질문): RequiredChoices 중 하나라도 이번 회차에 그 버튼으로 답했어야 출제
 */
UCLASS()
class OEC_API UOecQuestionFilter_Prerequisite : public UOecQuestionFilter
{
	GENERATED_BODY()

public:
	virtual bool PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const override;
};
