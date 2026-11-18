// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_AnswerHealth.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_AnswerHealth::UOecChoiceEffect_AnswerHealth()
{
	EffectTag = OecGameplayTags::Effect_Passive_AnswerHealth;
	StatEffectTag = OecGameplayTags::Effect_Stat_Health;
	StatName = TEXT("체력");
}
