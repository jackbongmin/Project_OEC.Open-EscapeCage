// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffect_StatChange.h"
#include "OecChoiceEffect_Sanity.generated.h"

/**
 * Effect.Stat.Sanity: 정신력 증감. Value = 증감량
 */
UCLASS()
class OEC_API UOecChoiceEffect_Sanity : public UOecChoiceEffect_StatChange
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Sanity();

protected:
	virtual TSoftClassPtr<UGameplayEffect> GetStatEffectClass() const override;
};
