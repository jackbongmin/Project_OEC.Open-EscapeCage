// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Selectors/OecQuestionSelector.h"
#include "OecQuestionSelector_FixedOrder.generated.h"

/**
 * 고정 순서 (튜토리얼): FixedOrder가 지정된 질문이 남아 있으면 작은 번호부터 먼저 낸다
 */
UCLASS()
class OEC_API UOecQuestionSelector_FixedOrder : public UOecQuestionSelector
{
	GENERATED_BODY()

public:
	UOecQuestionSelector_FixedOrder();

	virtual const FQuestionStaticData* SelectQuestion(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const override;
};
