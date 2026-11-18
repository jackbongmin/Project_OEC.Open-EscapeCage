// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffect_Charge.h"
#include "OecChoiceEffect_PeekOutcome.generated.h"

/**
 * Effect.Question.Peek (WARDEN의 메모): 다음 확률 질문의 결과를 미리 굴려 버튼에 보여 준다
 */
UCLASS()
class OEC_API UOecChoiceEffect_PeekOutcome : public UOecChoiceEffect_Charge
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_PeekOutcome();
};
