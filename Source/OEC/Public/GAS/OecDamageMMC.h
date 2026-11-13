// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "OecDamageMMC.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecDamageMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UOecDamageMMC();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
};
