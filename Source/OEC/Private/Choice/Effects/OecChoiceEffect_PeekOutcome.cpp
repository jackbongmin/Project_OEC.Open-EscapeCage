// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_PeekOutcome.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_PeekOutcome::UOecChoiceEffect_PeekOutcome()
{
	EffectTag = OecGameplayTags::Effect_Question_Peek;
	ChargeDescription = TEXT("다음 확률 결과 미리 보기");
}
