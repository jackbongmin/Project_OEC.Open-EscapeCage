// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_GiveItem.h"
#include "Actors/Room/OecChoiceSpawnPoint.h"
#include "Actors/Interactable/Item/OecItemPickupBase.h"
#include "Subsystems/OecSpawnPointSubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Settings/OecGameDataSettings.h"
#include "GAS/OecGameplayTags.h"

UOecChoiceEffect_GiveItem::UOecChoiceEffect_GiveItem()
{
	EffectTag = OecGameplayTags::Effect_Give_Item;
}

void UOecChoiceEffect_GiveItem::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	UWorld* world = GetContextWorld(InContext);
	UGameInstance* gameInstance = GetOwningGameInstance();
	if (!world || !gameInstance) return;

	const UOecGameDataSubsystem* dataSubsystem = gameInstance->GetSubsystem<UOecGameDataSubsystem>();
	const FItemStaticData* itemData = dataSubsystem ? dataSubsystem->GetItemData(InEffect.Param) : nullptr;
	if (!itemData)
	{
		UE_LOG(LogTemp, Error, TEXT("[Effect.Give.Item] 아이템 데이터가 없습니다: %s"), *InEffect.Param.ToString());
		return;
	}

	const int32 quantity = FMath::Max(FMath::RoundToInt(InEffect.Value), 1);

	// 1. 아이템 투하구가 있으면 픽업 액터를 떨어뜨림
	UOecSpawnPointSubsystem* spawnPointSubsystem = world->GetSubsystem<UOecSpawnPointSubsystem>();
	AOecChoiceSpawnPoint* spawnPoint = spawnPointSubsystem ? spawnPointSubsystem->GetRandomSpawnPoint(EOecSpawnPointType::Item) : nullptr;

	TSoftClassPtr<AOecItemPickupBase> pickupClassPtr = itemData->PickupActorClass.IsNull() ? GetDefault<UOecGameDataSettings>()->DefaultPickupClass : itemData->PickupActorClass;
	TSubclassOf<AOecItemPickupBase> pickupClass = pickupClassPtr.LoadSynchronous();

	if (spawnPoint && pickupClass)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (AOecItemPickupBase* pickup = world->SpawnActor<AOecItemPickupBase>(pickupClass, spawnPoint->GetRandomSpawnTransform(), spawnParams))
		{
			pickup->SetItemData(InEffect.Param, quantity);
			spawnPoint->BP_OnSpawned(pickup);
			UE_LOG(LogTemp, Log, TEXT("아이템 투하: %s x%d"), *itemData->ItemName, quantity);
			return;
		}
	}

	// 2. 투하구나 픽업 클래스가 없으면 인벤토리에 바로 지급
	if (UOecInventorySubsystem* inventorySubsystem = gameInstance->GetSubsystem<UOecInventorySubsystem>())
	{
		const bool bSuccess = inventorySubsystem->TryAddItem(InEffect.Param, quantity);
		UE_LOG(LogTemp, Log, TEXT("아이템 직접 지급: %s x%d (%s)"), *itemData->ItemName, quantity, bSuccess ? TEXT("성공") : TEXT("인벤토리 가득 참"));
	}
}

FString UOecChoiceEffect_GiveItem::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	const int32 quantity = FMath::Max(FMath::RoundToInt(InEffect.Value), 1);

	FString itemName = InEffect.Param.ToString();
	if (UGameInstance* gameInstance = GetOwningGameInstance())
	{
		if (const UOecGameDataSubsystem* dataSubsystem = gameInstance->GetSubsystem<UOecGameDataSubsystem>())
		{
			if (const FItemStaticData* itemData = dataSubsystem->GetItemData(InEffect.Param))
			{
				itemName = itemData->ItemName;
			}
		}
	}

	return quantity > 1 ? FString::Printf(TEXT("%s %d개 획득"), *itemName, quantity) : FString::Printf(TEXT("%s 획득"), *itemName);
}
