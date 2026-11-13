// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGA_UseItem.h"
#include "AbilitySystemComponent.h"
#include "Data/OecDataStruct.h"

UOecGA_UseItem::UOecGA_UseItem()
{
	AbilityInputID = EOecAbilityInputID::ItemUse;
}

void UOecGA_UseItem::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT(" [UOecGA_UseItem] 어빌리티 활성화됨!"));

	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

	FItemStaticData* ItemData = ItemDataRow.GetRow<FItemStaticData>(TEXT("GA_UseItem_Context"));

	if (!ItemData || !ItemEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ [UOecGA_UseItem] 데이터나 이펙트 클래스가 비어있음!"));
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = InActorInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ItemEffectClass, GetAbilityLevel(), ContextHandle);

		if (SpecHandle.IsValid())
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(ItemData->EffectTag, ItemData->HealthRestoreAmount);

			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}
