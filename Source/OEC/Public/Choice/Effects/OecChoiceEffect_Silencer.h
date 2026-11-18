// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffect_Charge.h"
#include "OecChoiceEffect_Silencer.generated.h"

/**
 * Effect.Guard.Silencer (소음 차단기): 다음 몬스터 소환 1회를 무효로 한다
 */
UCLASS()
class OEC_API UOecChoiceEffect_Silencer : public UOecChoiceEffect_Charge
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Silencer();
};
