// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Health.h"
#include "Settings/OecGameDataSettings.h"
#include "GAS/OecGameplayTags.h"
#include "GameplayEffect.h"

UOecChoiceEffect_Health::UOecChoiceEffect_Health()
{
	EffectTag = OecGameplayTags::Effect_Stat_Health;
	StatDisplayName = TEXT("체력");
}

TSoftClassPtr<UGameplayEffect> UOecChoiceEffect_Health::GetStatEffectClass() const
{
	return GetDefault<UOecGameDataSettings>()->ChoiceHealthEffect;
}
