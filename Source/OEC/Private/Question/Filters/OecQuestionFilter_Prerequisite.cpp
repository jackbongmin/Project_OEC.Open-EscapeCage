// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Filters/OecQuestionFilter_Prerequisite.h"
#include "Question/OecQuestionSubsystem.h"

bool UOecQuestionFilter_Prerequisite::PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const
{
	if (InQuestion.RequiredChoices.Num() == 0) return true;
	if (!InContext.QuestionSubsystem) return false;

	for (const FOecQuestionPrerequisite& prerequisite : InQuestion.RequiredChoices)
	{
		if (prerequisite.Button == EOecChoiceButton::None) continue;

		if (InContext.QuestionSubsystem->GetRunAnswer(prerequisite.QuestionID) == prerequisite.Button)
		{
			return true;
		}
	}

	return false;
}
