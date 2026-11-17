// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Filters/OecQuestionFilter.h"
#include "OecQuestionFilter_Used.generated.h"

/**
 * 중복 출제 방지
 * - 회차 단위 타입(튜토리얼, 스토리): 이번 회차에 답했으면 제외
 * - 그 외: 풀이 소진될 때까지 제외 (UOecEventSubsystem의 사용 기록)
 */
UCLASS()
class OEC_API UOecQuestionFilter_Used : public UOecQuestionFilter
{
	GENERATED_BODY()

public:
	virtual bool PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const override;
};
