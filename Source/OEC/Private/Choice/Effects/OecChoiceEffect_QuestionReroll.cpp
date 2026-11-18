// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_QuestionReroll.h"
#include "Question/OecQuestionSubsystem.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_QuestionReroll::UOecChoiceEffect_QuestionReroll()
{
	EffectTag = OecGameplayTags::Effect_Question_Reroll;
}

bool UOecChoiceEffect_QuestionReroll::CanExecute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	const UGameInstance* gameInstance = GetOwningGameInstance();
	const UOecQuestionSubsystem* questionSubsystem = gameInstance ? gameInstance->GetSubsystem<UOecQuestionSubsystem>() : nullptr;
	const bool bCanReroll = questionSubsystem && questionSubsystem->CanRerollCurrentQuestion();

	if (!bCanReroll)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Effect.Question.Reroll] 지금 질문은 바꿀 수 없습니다 (스토리·튜토리얼이거나 질문이 없음)"));
	}

	return bCanReroll;
}

void UOecChoiceEffect_QuestionReroll::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	UGameInstance* gameInstance = GetOwningGameInstance();
	if (UOecQuestionSubsystem* questionSubsystem = gameInstance ? gameInstance->GetSubsystem<UOecQuestionSubsystem>() : nullptr)
	{
		questionSubsystem->RequestReroll();
	}
}

FString UOecChoiceEffect_QuestionReroll::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return TEXT("지금 질문 교체");
}
