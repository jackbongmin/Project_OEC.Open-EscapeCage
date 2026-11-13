// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecQuickSlotPanelWidget.generated.h"

/**
 * 
 */

class UOecQuickSlotSlotWidget;

UCLASS()
class OEC_API UOecQuickSlotPanelWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnQuickSlotUpdated();

	UPROPERTY(meta = (BindWidget))
	UOecQuickSlotSlotWidget* Slot_1;

	UPROPERTY(meta = (BindWidget))
	UOecQuickSlotSlotWidget* Slot_2;

	UPROPERTY(meta = (BindWidget))
	UOecQuickSlotSlotWidget* Slot_3;

	UPROPERTY(meta = (BindWidget))
	UOecQuickSlotSlotWidget* Slot_4;

	UPROPERTY(meta = (BindWidget))
	UOecQuickSlotSlotWidget* Slot_5;

	UPROPERTY()
	TArray<UOecQuickSlotSlotWidget*> SlotArray;
};
