// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecInventoryPanelWidget.h"
#include "UI/Ingame/OecInventorySlotWidget.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Components/WrapBox.h"

void UOecInventoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotWidgetClass && InventoryWrapBox)
	{
		InventoryWrapBox->ClearChildren();
		SlotArray.Empty();

		for (int32 i = 0; i < 20; ++i)
		{
			UOecInventorySlotWidget* newSlot = CreateWidget<UOecInventorySlotWidget>(GetWorld(), SlotWidgetClass);
			if (newSlot)
			{
				newSlot->InitSlot(i);
				InventoryWrapBox->AddChildToWrapBox(newSlot); // UI 패널에 부착
				SlotArray.Add(newSlot); // 배열에 저장
			}
		}
	}

	// 인벤토리 서브시스템 구독 (아이템 먹을 때마다 갱신)
	if (UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>())
	{
		invenSub->OnInventoryUpdated.AddDynamic(this, &UOecInventoryPanelWidget::OnInventoryUpdated);
		OnInventoryUpdated(); 
	}
}

void UOecInventoryPanelWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (GetGameInstance())
	{
		if (UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>())
		{
			invenSub->OnInventoryUpdated.RemoveDynamic(this, &UOecInventoryPanelWidget::OnInventoryUpdated);
		}
	}
}

void UOecInventoryPanelWidget::OnInventoryUpdated()
{
	UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
	if (!invenSub) return;

	const TArray<FInventorySlot>& currentItems = invenSub->GetSlots();

	// 배열 돌면서 싹 다 새로고침
	for (int32 i = 0; i < SlotArray.Num(); ++i)
	{
		if (SlotArray[i])
		{

			// 플레이어가 가진 아이템 개수보다 i가 작으면 아이템 띄워주기
			if (i < currentItems.Num())
			{
				SlotArray[i]->RefreshSlot(currentItems[i].ItemCode, currentItems[i].Quantity);
			}
			else
			{
				// 넘어가면 빈 슬롯 처리
				SlotArray[i]->ClearSlot();
			}
		}
	}
}
