// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecInventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "System/OecItemDragDropOperation.h" // 드래그 데이터용
#include "Blueprint/WidgetBlueprintLibrary.h"

void UOecInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearSlot();
}


void UOecInventorySlotWidget::InitSlot(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

void UOecInventorySlotWidget::ClearSlot()
{
	CurrentItemCode = NAME_None;
	if (ItemIcon) ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	if (QuantityText) QuantityText->SetVisibility(ESlateVisibility::Hidden);
}

void UOecInventorySlotWidget::RefreshSlot(FName InItemCode, int32 InQuantity)
{
	CurrentItemCode = InItemCode; // 상태 업데이트

	if (InItemCode == NAME_None || InQuantity <= 0)
	{
		ClearSlot();
		return;
	}

	UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if (!dataSub) return;

	const FItemStaticData* itemData = dataSub->GetItemData(InItemCode);
	if (itemData && itemData->ItemIcon)
	{
		if (ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(itemData->ItemIcon);
			// 드래그가 끝났을 수도 있으니 투명도 원상복구
			ItemIcon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
			ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		if (QuantityText)
		{
			QuantityText->SetText(FText::AsNumber(InQuantity));
			QuantityText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void UOecInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	if (ItemIcon) ItemIcon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
}

FReply UOecInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	FReply reply = Super::NativeOnMouseButtonDown(InMyGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && CurrentItemCode != NAME_None)
	{
		return FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton);
	}
	return reply;
}

void UOecInventorySlotWidget::NativeOnDragDetected(const FGeometry& InMyGeometry, const FPointerEvent& InPointerEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InMyGeometry, InPointerEvent, OutOperation);

	UOecItemDragDropOperation* dragOp = NewObject<UOecItemDragDropOperation>();
	dragOp->ItemCode = CurrentItemCode;
	dragOp->SourceSlotIndex = SlotIndex;
	dragOp->bFromInventory = true;

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

	if (ItemIcon)
	{
		ItemIcon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.3f));
	}
}

bool UOecInventorySlotWidget::NativeOnDrop(const FGeometry& InMyGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InMyGeometry, InDragDropEvent, InOperation);

	UOecItemDragDropOperation* itemOp = Cast<UOecItemDragDropOperation>(InOperation);
	if (itemOp && itemOp->bFromInventory)
	{
		UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
		if (invenSub)
		{
			invenSub->SwapSlot(itemOp->SourceSlotIndex, SlotIndex);
			return true;
		}
	}
	return false;
}

void UOecInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InMyGeometry, InMouseEvent);
	if (CurrentItemCode != NAME_None)
	{
		// TODO: 오른쪽 아이콘 설명창 위젯 업데이트 로직 들어갈 곳
		UE_LOG(LogTemp, Log, TEXT("호버된 아이템: %s"), *CurrentItemCode.ToString());
	}
}

void UOecInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	// TODO: 설명창 숨기기 로직!
}


