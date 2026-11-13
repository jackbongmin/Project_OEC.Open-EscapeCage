// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Sanity.h"
#include "Settings/OecGameDataSettings.h"
#include "GAS/OecGameplayTags.h"
#include "GameplayEffect.h"

UOecChoiceEffect_Sanity::UOecChoiceEffect_Sanity()
{
	EffectTag = OecGameplayTags::Effect_Stat_Sanity;
	StatDisplayName = TEXT("정신력");
}

TSoftClassPtr<UGameplayEffect> UOecChoiceEffect_Sanity::GetStatEffectClass() const
{
	return GetDefault<UOecGameDataSettings>()->ChoiceSanityEffect;
}
