// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Silencer.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_Silencer::UOecChoiceEffect_Silencer()
{
	EffectTag = OecGameplayTags::Effect_Guard_Silencer;
	ChargeDescription = TEXT("다음 몬스터 소환 1회 무효");
}
