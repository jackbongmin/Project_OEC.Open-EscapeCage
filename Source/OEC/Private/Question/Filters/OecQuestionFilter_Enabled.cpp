// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Filters/OecQuestionFilter_Enabled.h"

bool UOecQuestionFilter_Enabled::PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const
{
	return InQuestion.bEnabled;
}
