// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Filters/OecQuestionFilter_Used.h"
#include "Question/OecQuestionSubsystem.h"
#include "Subsystems/OecEventSubsystem.h"
#include "Settings/OecGameDataSettings.h"

bool UOecQuestionFilter_Used::PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const
{
	// 회차마다 다시 나오는 타입은 이번 회차 기록만 본다
	if (InContext.Settings && InContext.Settings->RunScopedQuestionTypes.Contains(InQuestion.QuestionType))
	{
		return !InContext.QuestionSubsystem || !InContext.QuestionSubsystem->HasAnsweredThisRun(InQuestion.QuestionID);
	}

	return !InContext.EventSubsystem || !InContext.EventSubsystem->IsQuestionUsed(InQuestion.QuestionID);
}
