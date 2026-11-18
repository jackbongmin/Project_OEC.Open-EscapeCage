// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_QuestionReroll.generated.h"

/**
 * Effect.Question.Reroll (질문 변경권): 지금 단말기에 떠 있는 질문을 다른 질문으로 바꾼다
 * - 스토리·튜토리얼 질문은 바꿀 수 없다 (CanExecute = false → 아이템이 소모되지 않음)
 */
UCLASS()
class OEC_API UOecChoiceEffect_QuestionReroll : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_QuestionReroll();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
	virtual bool CanExecute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
};
