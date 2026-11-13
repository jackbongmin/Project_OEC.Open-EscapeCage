// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Stack.h"
#include "Choice/OecChoiceSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "Subsystems/OecEventSubsystem.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_Stack::UOecChoiceEffect_Stack()
{
	EffectTag = OecGameplayTags::Effect_Stack;
}

void UOecChoiceEffect_Stack::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	if (!InContext.ChoiceSubsystem) return;

	UGameInstance* gameInstance = InContext.ChoiceSubsystem->GetGameInstance();
	const int32 amount = FMath::RoundToInt(InEffect.Value);

	UOecStageManagerSubsystem* stageManager = gameInstance->GetSubsystem<UOecStageManagerSubsystem>();

	if (stageManager && stageManager->GetCurrentPhase() == EOecGamePhase::Phase1_MainRoom)
	{
		// 페이즈 1: 전구 스택 (소모되지 않음, 다 차면 페이즈 2로 전환)
		stageManager->AddStack(amount);
	}
	else if (UOecEventSubsystem* eventSubsystem = gameInstance->GetSubsystem<UOecEventSubsystem>())
	{
		// 페이즈 2: 문 열쇠 스택 (닫힌 방 문을 열 때 소모)
		eventSubsystem->ModifyStacks(amount);
	}
}

FString UOecChoiceEffect_Stack::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	const int32 amount = FMath::RoundToInt(InEffect.Value);
	return amount >= 0 ? FString::Printf(TEXT("스택 +%d"), amount) : FString::Printf(TEXT("스택 %d"), amount);
}
