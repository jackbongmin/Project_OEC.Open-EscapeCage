// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Stack.h"
#include "Choice/OecChoiceSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_Stack::UOecChoiceEffect_Stack()
{
	EffectTag = OecGameplayTags::Effect_Stack;
}

void UOecChoiceEffect_Stack::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	if (!InContext.ChoiceSubsystem) return;

	// 페이즈 1: 전구 스택, 페이즈 2: 열쇠 아이템 (스테이지 서브시스템이 판단)
	if (UOecStageManagerSubsystem* stageManager = InContext.ChoiceSubsystem->GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>())
	{
		stageManager->AddStack(FMath::RoundToInt(InEffect.Value));
	}
}

FString UOecChoiceEffect_Stack::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	const int32 amount = FMath::RoundToInt(InEffect.Value);

	const UGameInstance* gameInstance = GetOwningGameInstance();
	const UOecStageManagerSubsystem* stageManager = gameInstance ? gameInstance->GetSubsystem<UOecStageManagerSubsystem>() : nullptr;
	const bool bIsPhase2 = stageManager && stageManager->GetCurrentPhase() == EOecGamePhase::Phase2_Corridor;

	if (bIsPhase2)
	{
		return amount >= 0 ? FString::Printf(TEXT("열쇠 %d개 획득"), amount) : FString::Printf(TEXT("열쇠 %d개 잃음"), -amount);
	}

	return amount >= 0 ? FString::Printf(TEXT("스택 +%d"), amount) : FString::Printf(TEXT("스택 %d"), amount);
}
