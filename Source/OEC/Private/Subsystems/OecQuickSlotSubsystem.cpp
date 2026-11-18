// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecQuickSlotSubsystem.h"
#include "Subsystems/OecRunSubsystem.h"

void UOecQuickSlotSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	QuickSlots.Empty();

	if (UOecRunSubsystem* runSubsystem = Collection.InitializeDependency<UOecRunSubsystem>())
	{
		runSubsystem->OnRunStarted.AddDynamic(this, &UOecQuickSlotSubsystem::OnRunStarted);
	}
}

void UOecQuickSlotSubsystem::OnRunStarted(int32 InRunCount)
{
	QuickSlots.Empty();
	OnQuickSlotUpdated.Broadcast();
}

void UOecQuickSlotSubsystem::assignQuickSlot(int32 InSlotIndex, FName InItemID)
{
	const int32* existingSlot = QuickSlots.FindKey(InItemID);

	if(existingSlot && *existingSlot != InSlotIndex)
	{
		QuickSlots.Remove(*existingSlot);
	}
	QuickSlots.Add(InSlotIndex, InItemID);
	OnQuickSlotUpdated.Broadcast();
}

void UOecQuickSlotSubsystem::ClearQuickSlot(int32 InSlotIndex)
{
	if (QuickSlots.Contains(InSlotIndex))
	{
		QuickSlots.Remove(InSlotIndex);
		OnQuickSlotUpdated.Broadcast();
	}
}

FName UOecQuickSlotSubsystem::GetItemAtSlot(int32 InSlotIndex) const
{
	if (const FName* foundItem = QuickSlots.Find(InSlotIndex))
	{
		return *foundItem;
	}
	return NAME_None;
}
