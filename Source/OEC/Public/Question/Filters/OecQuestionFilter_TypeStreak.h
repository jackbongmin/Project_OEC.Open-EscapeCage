// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Filters/OecQuestionFilter.h"
#include "OecQuestionFilter_TypeStreak.generated.h"

/**
 * 연속 제한: 같은 타입이 설정한 횟수 넘게 연달아 나오지 않게 함 (예: 같은 타입 3연속, 손해 2연속 금지)
 * 후보가 없으면 건너뛰는 완화 가능 필터
 */
UCLASS()
class OEC_API UOecQuestionFilter_TypeStreak : public UOecQuestionFilter
{
	GENERATED_BODY()

public:
	UOecQuestionFilter_TypeStreak();

	virtual bool PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const override;
};
