// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecInventorySubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"

void UOecInventorySubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);
	InventorySlots.Empty();
	InventorySlots.SetNum(MaxSlotCount);
}

bool UOecInventorySubsystem::TryAddItem(FName InItemCode, int32 InCount)
{
	// 존재하는 아이템인지 확인
	UOecGameDataSubsystem* dataSubsystem = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if(!dataSubsystem||!dataSubsystem->GetItemData(InItemCode))
	{
		UE_LOG(LogTemp, Warning, TEXT("존재하지 않는 아이템 코드: %s"), *InItemCode.ToString());
		return false;
	}

	// 이미 가지고 있는 아이템인지 확인
	for(FInventorySlot& slot : InventorySlots)
	{
		if (slot.ItemCode == InItemCode)
		{
			slot.Quantity += InCount;
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	for (FInventorySlot& slot : InventorySlots)
	{
		if (slot.ItemCode == NAME_None)
		{
			slot.ItemCode = InItemCode;
			slot.Quantity = InCount;
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("인벤토리가 가득 찼습니다!"));
	return false;
}

void UOecInventorySubsystem::RemoveItem(FName InItemCode, int32 InCount)
{
	for (FInventorySlot& slot : InventorySlots)
	{
		if (slot.ItemCode == InItemCode)
		{
			slot.Quantity -= InCount;

			if (slot.Quantity <= 0)
			{
				slot.ItemCode = NAME_None;
				slot.Quantity = 0;
			}

			OnInventoryUpdated.Broadcast();
			return;
		}
	}
}

void UOecInventorySubsystem::SwapSlot(int32 InSourceIndex, int32 InDestinationIndex)
{
	// 인덱스가 정상적인 범위인지 확인
	if (!InventorySlots.IsValidIndex(InSourceIndex) || !InventorySlots.IsValidIndex(InDestinationIndex)) return;

	// 자기 자신에게 드롭한 거면 무시
	if (InSourceIndex == InDestinationIndex) return;

	// 데이터 맞교환 (Swap)
	FInventorySlot tempSlot = InventorySlots[InSourceIndex];
	InventorySlots[InSourceIndex] = InventorySlots[InDestinationIndex];
	InventorySlots[InDestinationIndex] = tempSlot;

	// UI 갱신 방송 쏘기!
	OnInventoryUpdated.Broadcast();
}

int32 UOecInventorySubsystem::GetItemQuantity(FName InItemCode) const
{
	for (const FInventorySlot& slot : InventorySlots)
	{
		if (slot.ItemCode == InItemCode)
		{
			return slot.Quantity;
		}
	}
	return 0;
}
