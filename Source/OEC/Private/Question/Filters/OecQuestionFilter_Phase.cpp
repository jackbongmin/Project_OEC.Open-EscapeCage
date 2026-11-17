// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Filters/OecQuestionFilter_Phase.h"

bool UOecQuestionFilter_Phase::PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const
{
	return InQuestion.AllowedPhases.Num() == 0 || InQuestion.AllowedPhases.Contains(InContext.PhaseNumber);
}
