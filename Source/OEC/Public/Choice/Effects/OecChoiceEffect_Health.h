// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffect_StatChange.h"
#include "OecChoiceEffect_Health.generated.h"

/**
 * Effect.Stat.Health: 체력 증감. Value = 증감량
 */
UCLASS()
class OEC_API UOecChoiceEffect_Health : public UOecChoiceEffect_StatChange
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Health();

protected:
	virtual TSoftClassPtr<UGameplayEffect> GetStatEffectClass() const override;
};
