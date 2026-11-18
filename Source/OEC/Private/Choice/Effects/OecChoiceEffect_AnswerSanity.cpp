// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_AnswerSanity.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_AnswerSanity::UOecChoiceEffect_AnswerSanity()
{
	EffectTag = OecGameplayTags::Effect_Passive_AnswerSanity;
	StatEffectTag = OecGameplayTags::Effect_Stat_Sanity;
	StatName = TEXT("정신력");
}
