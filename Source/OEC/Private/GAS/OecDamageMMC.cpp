// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/OecDamageMMC.h"
#include "GAS/OecGameplayTags.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"
#include "AbilitySystemComponent.h"

UOecDamageMMC::UOecDamageMMC()
{
	AttackPowerDef.AttributeToCapture = UOecAttributeSetBase::GetAttackPowerAttribute();
	AttackPowerDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source; 
	AttackPowerDef.bSnapshot = false; 


	RelevantAttributesToCapture.Add(AttackPowerDef);
}

float UOecDamageMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const
{
	const FGameplayTagContainer* SourceTags = InSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = InSpec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float currentAttackPower = 0.0f;
	GetCapturedAttributeMagnitude(AttackPowerDef, InSpec, EvaluationParameters, currentAttackPower);

	currentAttackPower = FMath::Max<float>(currentAttackPower, 0.0f);

	float weaponBaseDamage = InSpec.GetSetByCallerMagnitude(OecGameplayTags::Data_Damage, false, 0.0f);

	float finalDamage = weaponBaseDamage * (1.0f + (currentAttackPower / 100.0f));

	UE_LOG(LogTemp, Warning, TEXT("💥 [데미지 계산] 무기(%f) * 내스탯(%f) = 최종 데미지: %f"), weaponBaseDamage, currentAttackPower, finalDamage);

	return finalDamage;
}
