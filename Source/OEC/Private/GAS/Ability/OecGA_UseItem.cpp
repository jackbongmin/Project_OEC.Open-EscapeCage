// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGA_UseItem.h"
#include "GAS/OecGameplayTags.h"
#include "Data/OecDataStruct.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Choice/OecChoiceSubsystem.h"

UOecGA_UseItem::UOecGA_UseItem()
{
	// 입력이 아니라 Event.Item.Use 이벤트로 발동 (어떤 아이템인지 함께 받기 위함)
	FAbilityTriggerData triggerData;
	triggerData.TriggerTag = OecGameplayTags::Event_Item_Use;
	triggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerData);
}

void UOecGA_UseItem::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

	AActor* avatar = InActorInfo ? InActorInfo->AvatarActor.Get() : nullptr;
	const UOecItemUsePayload* payload = InTriggerEventData ? Cast<UOecItemUsePayload>(InTriggerEventData->OptionalObject) : nullptr;
	const UGameInstance* gameInstance = avatar ? avatar->GetGameInstance() : nullptr;
	const UOecGameDataSubsystem* dataSubsystem = gameInstance ? gameInstance->GetSubsystem<UOecGameDataSubsystem>() : nullptr;
	const FItemStaticData* itemData = (payload && dataSubsystem) ? dataSubsystem->GetItemData(payload->ItemCode) : nullptr;

	if (!itemData)
	{
		UE_LOG(LogTemp, Error, TEXT("[UOecGA_UseItem] 사용할 아이템 정보가 없습니다 (이벤트에 UOecItemUsePayload가 있는지 확인)"));
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	ApplyItemEffects(*itemData, avatar);
	UE_LOG(LogTemp, Log, TEXT("[UOecGA_UseItem] %s 사용"), *itemData->ItemName);

	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}

void UOecGA_UseItem::ApplyItemEffects(const FItemStaticData& InItemData, AActor* InAvatar) const
{
	UOecChoiceSubsystem* choiceSubsystem = InAvatar->GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>();
	if (!choiceSubsystem) return;

	// 1. 회복량 필드 → 체력·정신력 효과 (GE + SetByCaller)
	if (!FMath::IsNearlyZero(InItemData.HealthRestoreAmount))
	{
		FOecChoiceEffect healthEffect;
		healthEffect.EffectTag = OecGameplayTags::Effect_Stat_Health;
		healthEffect.Value = InItemData.HealthRestoreAmount;
		choiceSubsystem->ExecuteEffect(healthEffect, InAvatar);
	}

	if (!FMath::IsNearlyZero(InItemData.SanityRestoreAmount))
	{
		FOecChoiceEffect sanityEffect;
		sanityEffect.EffectTag = OecGameplayTags::Effect_Stat_Sanity;
		sanityEffect.Value = InItemData.SanityRestoreAmount;
		choiceSubsystem->ExecuteEffect(sanityEffect, InAvatar);
	}

	// 2. 추가 효과
	for (const FOecChoiceEffect& useEffect : InItemData.UseEffects)
	{
		choiceSubsystem->ExecuteEffect(useEffect, InAvatar);
	}
}
