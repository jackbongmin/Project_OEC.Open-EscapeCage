// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Selectors/OecQuestionSelector_StackPity.h"
#include "Settings/OecGameDataSettings.h"
#include "GAS/OecGameplayTags.h"

UOecQuestionSelector_StackPity::UOecQuestionSelector_StackPity()
{
	Priority = 200;
}

const FQuestionStaticData* UOecQuestionSelector_StackPity::SelectQuestion(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const
{
	const int32 pityCount = InContext.Settings ? InContext.Settings->StackPityAnswerCount : 0;
	if (pityCount <= 0 || InContext.AnswersWithoutStack < pityCount) return nullptr;

	// 단말기에 가중치가 있는 타입 중에서, 어느 한쪽 선택지가 스택을 100% 주는 질문만 고른다
	TArray<const FQuestionStaticData*> guaranteedQuestions;
	for (const FQuestionStaticData* candidate : InCandidates)
	{
		if (candidate->FixedOrder > 0 || InContext.GetTypeWeight(candidate->QuestionType) <= 0.0f) continue;

		if (HasGuaranteedStack(candidate->RedChoice) || HasGuaranteedStack(candidate->BlueChoice))
		{
			guaranteedQuestions.Add(candidate);
		}
	}

	if (guaranteedQuestions.Num() == 0) return nullptr;

	UE_LOG(LogTemp, Log, TEXT("스택 보정 발동: %d회 연속 스택 미획득"), InContext.AnswersWithoutStack);
	return guaranteedQuestions[FMath::RandRange(0, guaranteedQuestions.Num() - 1)];
}

bool UOecQuestionSelector_StackPity::HasGuaranteedStack(const FOecChoiceData& InChoice) const
{
	bool bHasOutcome = false;
	for (const FOecChoiceOutcome& outcome : InChoice.Outcomes)
	{
		if (outcome.Weight <= 0.0f) continue;

		const bool bGivesStack = outcome.Effects.ContainsByPredicate([](const FOecChoiceEffect& InEffect)
		{
			return InEffect.EffectTag == OecGameplayTags::Effect_Stack && InEffect.Value > 0.0f;
		});

		if (!bGivesStack) return false;

		bHasOutcome = true;
	}

	return bHasOutcome;
}
