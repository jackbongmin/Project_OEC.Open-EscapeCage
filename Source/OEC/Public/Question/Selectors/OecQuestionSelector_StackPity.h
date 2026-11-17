// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Selectors/OecQuestionSelector.h"
#include "OecQuestionSelector_StackPity.generated.h"

/**
 * 스택 보정 (천장): 설정한 횟수만큼 연속으로 스택을 못 얻으면 확정 스택 선택지가 있는 질문을 낸다
 * 운이 나쁜 판이 끝없이 늘어지는 것만 막는다 (QuestionGuideline.md 6장)
 */
UCLASS()
class OEC_API UOecQuestionSelector_StackPity : public UOecQuestionSelector
{
	GENERATED_BODY()

public:
	UOecQuestionSelector_StackPity();

	virtual const FQuestionStaticData* SelectQuestion(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const override;

private:
	// 어떤 결과가 뽑혀도 스택을 얻는 선택지인지
	bool HasGuaranteedStack(const FOecChoiceData& InChoice) const;
};
