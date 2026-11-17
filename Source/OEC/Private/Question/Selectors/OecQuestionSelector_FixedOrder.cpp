// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/Selectors/OecQuestionSelector_FixedOrder.h"

UOecQuestionSelector_FixedOrder::UOecQuestionSelector_FixedOrder()
{
	Priority = 300;
}

const FQuestionStaticData* UOecQuestionSelector_FixedOrder::SelectQuestion(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const
{
	const FQuestionStaticData* selected = nullptr;
	for (const FQuestionStaticData* candidate : InCandidates)
	{
		if (candidate->FixedOrder <= 0) continue;

		if (!selected || candidate->FixedOrder < selected->FixedOrder)
		{
			selected = candidate;
		}
	}

	return selected;
}
