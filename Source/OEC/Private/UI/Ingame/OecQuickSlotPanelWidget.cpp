// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecQuickSlotPanelWidget.h"
#include "UI/Ingame/OecQuickSlotSlotWidget.h"
#include "Subsystems/OecQuickSlotSubsystem.h"

void UOecQuickSlotPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotArray.Empty();
	SlotArray.Add(Slot_1);
	SlotArray.Add(Slot_2);
	SlotArray.Add(Slot_3);
	SlotArray.Add(Slot_4);
	SlotArray.Add(Slot_5);

	// 각 슬롯 번호 초기화
	for (int32 i = 0; i < SlotArray.Num(); ++i)
	{
		if (SlotArray[i])
		{
			SlotArray[i]->InitSlot(i + 1); 
		}
	}

	if (UOecQuickSlotSubsystem* quickSlotSub = GetGameInstance()->GetSubsystem<UOecQuickSlotSubsystem>())
	{
		quickSlotSub->OnQuickSlotUpdated.AddDynamic(this, &UOecQuickSlotPanelWidget::OnQuickSlotUpdated);
	}
}

void UOecQuickSlotPanelWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (GetGameInstance())
	{
		if (UOecQuickSlotSubsystem* quickSlotSub = GetGameInstance()->GetSubsystem<UOecQuickSlotSubsystem>())
		{
			quickSlotSub->OnQuickSlotUpdated.RemoveDynamic(this, &UOecQuickSlotPanelWidget::OnQuickSlotUpdated);
		}
	}
}

void UOecQuickSlotPanelWidget::OnQuickSlotUpdated()
{
	UOecQuickSlotSubsystem* quickSlotSub = GetGameInstance()->GetSubsystem<UOecQuickSlotSubsystem>();
	if (!quickSlotSub) return;

	for (int32 i = 0; i < SlotArray.Num(); ++i)
	{
		if (SlotArray[i])
		{
			FName itemCode = quickSlotSub->GetItemAtSlot(i + 1);
			SlotArray[i]->RefreshSlot(itemCode);
		}
	}
}
