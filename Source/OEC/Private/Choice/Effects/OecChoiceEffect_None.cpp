// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_None.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_None::UOecChoiceEffect_None()
{
	EffectTag = OecGameplayTags::Effect_None;
}

void UOecChoiceEffect_None::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	UE_LOG(LogTemp, Log, TEXT("아무 일도 일어나지 않았습니다"));
}

FString UOecChoiceEffect_None::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return TEXT("아무 일도 일어나지 않음");
}
