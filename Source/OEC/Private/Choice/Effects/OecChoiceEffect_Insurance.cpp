// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Insurance.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_Insurance::UOecChoiceEffect_Insurance()
{
	EffectTag = OecGameplayTags::Effect_Guard_Insurance;
	ChargeDescription = TEXT("나쁜 확률 결과 1회 다시 굴림");
}
