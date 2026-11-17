// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Filters/OecQuestionFilter_RunCount.h"

bool UOecQuestionFilter_RunCount::PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const
{
	return InQuestion.MinRunCount <= InContext.RunCount;
}
