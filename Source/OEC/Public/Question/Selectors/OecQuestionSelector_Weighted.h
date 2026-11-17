// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Question/Selectors/OecQuestionSelector.h"
#include "OecQuestionSelector_Weighted.generated.h"

/**
 * 기본 출제: 단말기의 타입별 가중치로 타입을 먼저 뽑고, 그 타입 안에서 무작위로 하나를 낸다
 */
UCLASS()
class OEC_API UOecQuestionSelector_Weighted : public UOecQuestionSelector
{
	GENERATED_BODY()

public:
	UOecQuestionSelector_Weighted();

	virtual const FQuestionStaticData* SelectQuestion(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const override;
};
