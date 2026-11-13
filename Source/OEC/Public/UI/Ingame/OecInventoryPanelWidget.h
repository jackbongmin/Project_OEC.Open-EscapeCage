// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecInventoryPanelWidget.generated.h"

/**
 * 
 */

class UOecInventorySlotWidget;
class UWrapBox;

UCLASS()
class OEC_API UOecInventoryPanelWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnInventoryUpdated();

	UPROPERTY(EditDefaultsOnly, Category = "OEC|UI")
	TSubclassOf<UOecInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryWrapBox;

	UPROPERTY()
	TArray<UOecInventorySlotWidget*> SlotArray;

};
