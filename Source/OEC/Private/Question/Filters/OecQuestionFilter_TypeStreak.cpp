// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Filters/OecQuestionFilter_TypeStreak.h"
#include "Question/OecQuestionSubsystem.h"
#include "Settings/OecGameDataSettings.h"

UOecQuestionFilter_TypeStreak::UOecQuestionFilter_TypeStreak()
{
	bRelaxable = true;
}

bool UOecQuestionFilter_TypeStreak::PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const
{
	if (!InContext.Settings || !InContext.QuestionSubsystem) return true;

	const int32* typeLimit = InContext.Settings->QuestionTypeStreakLimits.Find(InQuestion.QuestionType);
	const int32 streakLimit = typeLimit ? *typeLimit : InContext.Settings->DefaultQuestionTypeStreakLimit;
	if (streakLimit <= 0) return true;

	// 직전 답변부터 거슬러 올라가며 같은 타입이 몇 번 이어졌는지 센다
	const TArray<EQuestionType>& history = InContext.QuestionSubsystem->GetAnswerTypeHistory();
	int32 streak = 0;
	for (int32 i = history.Num() - 1; i >= 0 && history[i] == InQuestion.QuestionType; --i)
	{
		++streak;
	}

	return streak < streakLimit;
}
