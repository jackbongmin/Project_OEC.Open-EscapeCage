// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OecCheatManager.h"
#include "Choice/OecChoiceSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "Subsystems/OecEventSubsystem.h"
#include "GAS/OecGameplayTags.h"
#include "GAS/Attributeset/OecPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"

void UOecCheatManager::OecAddStack(int32 InAmount)
{
	RunEffect(OecGameplayTags::Effect_Stack, InAmount, NAME_None);
}

void UOecCheatManager::OecSetPhase(int32 InPhase)
{
	UOecStageManagerSubsystem* stageManager = GetWorld()->GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>();
	if (!stageManager) return;

	stageManager->ChangePhase(InPhase >= 2 ? EOecGamePhase::Phase2_Corridor : EOecGamePhase::Phase1_MainRoom);
	UE_LOG(LogTemp, Warning, TEXT("[Cheat] 페이즈 %d로 변경"), InPhase >= 2 ? 2 : 1);
}

void UOecCheatManager::OecSetHealth(float InValue)
{
	if (UAbilitySystemComponent* asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerPawn()))
	{
		asc->SetNumericAttributeBase(UOecPlayerAttributeSet::GetHealthAttribute(), InValue);
		UE_LOG(LogTemp, Warning, TEXT("[Cheat] 체력 %g"), InValue);
	}
}

void UOecCheatManager::OecSetSanity(float InValue)
{
	if (UAbilitySystemComponent* asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerPawn()))
	{
		asc->SetNumericAttributeBase(UOecPlayerAttributeSet::GetSanityAttribute(), InValue);
		UE_LOG(LogTemp, Warning, TEXT("[Cheat] 정신력 %g"), InValue);
	}
}

void UOecCheatManager::OecSpawnMonster(FName InMonsterCode, int32 InCount)
{
	RunEffect(OecGameplayTags::Effect_Spawn_Monster, InCount, InMonsterCode);
}

void UOecCheatManager::OecGiveItem(FName InItemCode, int32 InCount)
{
	RunEffect(OecGameplayTags::Effect_Give_Item, InCount, InItemCode);
}

void UOecCheatManager::OecRunEffect(const FString& InEffectTagName, float InValue, FName InParam)
{
	// 콘솔 입력 문자열을 태그로 바꾸는 곳이라 예외적으로 문자열 조회 사용
	const FGameplayTag effectTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*InEffectTagName), false);
	if (!effectTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Cheat] 존재하지 않는 태그입니다: %s"), *InEffectTagName);
		return;
	}

	RunEffect(effectTag, InValue, InParam);
}

void UOecCheatManager::OecShowState()
{
	UGameInstance* gameInstance = GetWorld()->GetGameInstance();

	if (const UOecStageManagerSubsystem* stageManager = gameInstance->GetSubsystem<UOecStageManagerSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[State] 페이즈 %d | 전구 %d / %d"),
			stageManager->GetCurrentPhase() == EOecGamePhase::Phase1_MainRoom ? 1 : 2, stageManager->GetCurrentStack(), stageManager->GetMaxStack());
	}

	if (const UOecEventSubsystem* eventSubsystem = gameInstance->GetSubsystem<UOecEventSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[State] 열쇠 스택 %d"), eventSubsystem->GetCurrentStacks());
	}

	if (const UAbilitySystemComponent* asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[State] 체력 %g | 정신력 %g"),
			asc->GetNumericAttribute(UOecPlayerAttributeSet::GetHealthAttribute()), asc->GetNumericAttribute(UOecPlayerAttributeSet::GetSanityAttribute()));
	}

	if (const UOecChoiceSubsystem* choiceSubsystem = GetChoiceSubsystem())
	{
		UE_LOG(LogTemp, Warning, TEXT("[State] 누적 선택 %d회 | 지속 효과 %d개"), choiceSubsystem->GetChoiceHistory().Num(), choiceSubsystem->GetAnswerModifiers().Num());

		for (const FOecAnswerModifier& modifier : choiceSubsystem->GetAnswerModifiers())
		{
			UE_LOG(LogTemp, Warning, TEXT("[State]   - %s (값 %g, 남은 답변 %d)"), *modifier.EffectTag.ToString(), modifier.Value, modifier.RemainingAnswers);
		}
	}
}

void UOecCheatManager::RunEffect(const FGameplayTag& InEffectTag, float InValue, FName InParam)
{
	UOecChoiceSubsystem* choiceSubsystem = GetChoiceSubsystem();
	if (!choiceSubsystem) return;

	FOecChoiceEffect effect;
	effect.EffectTag = InEffectTag;
	effect.Value = InValue;
	effect.Param = InParam;

	FOecChoiceOutcome outcome;
	outcome.Effects.Add(effect);

	FOecChoiceData choice;
	choice.Outcomes.Add(outcome);

	UE_LOG(LogTemp, Warning, TEXT("[Cheat] 효과 실행: %s"), *choiceSubsystem->BuildChoiceText(choice));
	choiceSubsystem->ExecuteEffect(effect, GetPlayerPawn());
}

UOecChoiceSubsystem* UOecCheatManager::GetChoiceSubsystem() const
{
	return GetWorld()->GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>();
}

APawn* UOecCheatManager::GetPlayerPawn() const
{
	const APlayerController* playerController = GetOuterAPlayerController();
	return playerController ? playerController->GetPawn() : nullptr;
}
