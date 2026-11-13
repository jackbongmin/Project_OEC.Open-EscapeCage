// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecQuickSlotSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Subsystems/OecGameDataSubsystem.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Subsystems/OecQuickSlotSubsystem.h"

#include "System/OecItemDragDropOperation.h"

void UOecQuickSlotSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (ItemIcon)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	if (UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>())
	{
		invenSub->OnInventoryUpdated.AddDynamic(this, &UOecQuickSlotSlotWidget::HandleInventoryUpdate);
	}
}
bool UOecQuickSlotSlotWidget::NativeOnDrop(const FGeometry& InMyGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InMyGeometry, InDragDropEvent, InOperation);

	UOecItemDragDropOperation* itemOp = Cast<UOecItemDragDropOperation>(InOperation);


	if (itemOp)
	{
		UOecQuickSlotSubsystem* quickSub = GetGameInstance()->GetSubsystem<UOecQuickSlotSubsystem>();
		if (quickSub)
		{
			quickSub->assignQuickSlot(SlotIndex, itemOp->ItemCode);

			if (UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>())
			{
				invenSub->OnInventoryUpdated.Broadcast();
			}

			return true;
		}
	}

	return false;
}
FReply UOecQuickSlotSlotWidget::NativeOnMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	FReply reply = Super::NativeOnMouseButtonDown(InMyGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && CurrentItemCode != NAME_None)
	{
		return FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton);
	}
	return reply;
}
void UOecQuickSlotSlotWidget::NativeOnDragDetected(const FGeometry& InMyGeometry, const FPointerEvent& InPointerEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InMyGeometry, InPointerEvent, OutOperation);

	UOecItemDragDropOperation* dragOp = NewObject<UOecItemDragDropOperation>();
	dragOp->ItemCode = CurrentItemCode;
	dragOp->SourceSlotIndex = SlotIndex;
	dragOp->bFromInventory = false;

	UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if (dataSub)
	{
		const FItemStaticData* itemData = dataSub->GetItemData(CurrentItemCode);
		if (itemData && itemData->ItemIcon)
		{
			UImage* DragVisual = NewObject<UImage>();
			FSlateBrush Brush;
			Brush.SetResourceObject(itemData->ItemIcon);
			Brush.ImageSize = FVector2D(80.f, 80.f);
			DragVisual->SetBrush(Brush);

			dragOp->DefaultDragVisual = DragVisual;
			dragOp->Pivot = EDragPivot::CenterCenter;
		}
	}

	OutOperation = dragOp;


	if (ItemIcon) ItemIcon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.3f));
}

void UOecQuickSlotSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	if (ItemIcon)
	{
		ItemIcon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	}
}
void UOecQuickSlotSlotWidget::HandleInventoryUpdate()
{
	RefreshSlot(CurrentItemCode);
}
void UOecQuickSlotSlotWidget::InitSlot(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	if (SlotIndexText)
	{
		SlotIndexText->SetText(FText::AsNumber(SlotIndex));
		
	}
}

void UOecQuickSlotSlotWidget::RefreshSlot(FName InItemID)
{
	CurrentItemCode = InItemID;

	if (InItemID.IsNone())
	{
		if (ItemIcon)
		{
			ItemIcon->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
	}

	UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
	int32 currentCount = invenSub ? invenSub->GetItemQuantity(InItemID) : 0;

	if (currentCount <= 0)
	{
		if (ItemIcon) ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		return; 
	}

	UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if (dataSub)
	{
		const FItemStaticData* itemData = dataSub->GetItemData(InItemID);

		
		if(itemData && itemData->ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(itemData->ItemIcon);
			ItemIcon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
			ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		

		// 임시로 작동 확인용 (아이템 있으면 보이게 처리)
		if (ItemIcon) ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	}
}

