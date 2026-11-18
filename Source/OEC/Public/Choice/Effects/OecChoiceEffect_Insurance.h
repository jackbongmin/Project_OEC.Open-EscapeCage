// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffect_Charge.h"
#include "OecChoiceEffect_Insurance.generated.h"

/**
 * Effect.Guard.Insurance (보험 증서): 다음 확률 결과가 나쁘면 한 번 다시 굴린다
 */
UCLASS()
class OEC_API UOecChoiceEffect_Insurance : public UOecChoiceEffect_Charge
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Insurance();
};
