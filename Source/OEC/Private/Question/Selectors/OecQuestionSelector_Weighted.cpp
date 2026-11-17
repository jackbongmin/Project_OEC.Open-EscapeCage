// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Selectors/OecQuestionSelector_Weighted.h"

UOecQuestionSelector_Weighted::UOecQuestionSelector_Weighted()
{
	Priority = 100;
}

const FQuestionStaticData* UOecQuestionSelector_Weighted::SelectQuestion(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const
{
	// 1. 후보를 타입별로 분류 (가중치가 없는 타입과 고정 순서 질문은 제외)
	TMap<EQuestionType, TArray<const FQuestionStaticData*>> categorizedQuestions;
	for (const FQuestionStaticData* candidate : InCandidates)
	{
		if (candidate->FixedOrder > 0 || InContext.GetTypeWeight(candidate->QuestionType) <= 0.0f) continue;

		categorizedQuestions.FindOrAdd(candidate->QuestionType).Add(candidate);
	}

	float totalWeight = 0.0f;
	for (const auto& pair : categorizedQuestions)
	{
		totalWeight += InContext.GetTypeWeight(pair.Key);
	}

	if (totalWeight <= 0.0f) return nullptr;

	// 2. 타입 뽑기
	const float randomRoll = FMath::FRandRange(0.0f, totalWeight);
	float accumulatedWeight = 0.0f;
	const TArray<const FQuestionStaticData*>* selectedQuestions = nullptr;

	for (const auto& pair : categorizedQuestions)
	{
		selectedQuestions = &pair.Value;
		accumulatedWeight += InContext.GetTypeWeight(pair.Key);
		if (randomRoll <= accumulatedWeight) break;
	}

	// 3. 뽑힌 타입 안에서 질문 하나
	return (*selectedQuestions)[FMath::RandRange(0, selectedQuestions->Num() - 1)];
}
