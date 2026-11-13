// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecInventorySlotWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UOecDragDropOperation;

UCLASS()
class OEC_API UOecInventorySlotWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
public:
	void InitSlot(int32 InSlotIndex);
	void RefreshSlot(FName InItemCode, int32 InQuantity);

	void ClearSlot();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	// 마우스 클릭 감지 (드래그 준비)
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	// 실제 드래그 시작 (반투명화 및 데이터 담기)
	virtual void NativeOnDragDetected(const FGeometry& InMyGeometry, const FPointerEvent& InPointerEvent, UDragDropOperation*& OutOperation) override;
	// 아이템을 내려놓았을 때 (위치 교체)
	virtual bool NativeOnDrop(const FGeometry& InMyGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	// 마우스 호버 (설명창 띄우기 준비)
	virtual void NativeOnMouseEnter(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuantityText;

	int32 SlotIndex;
	FName CurrentItemCode;

};
