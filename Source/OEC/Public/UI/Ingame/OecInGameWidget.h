// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecInGameWidget.generated.h"

/**
 * 
 */

class UOecQuickSlotPanelWidget;
class UOecCrosshairWidget;
class UOecStatPanelWidget;
class UOecWeaponPanelWidget;

UCLASS()
class OEC_API UOecInGameWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
public:
	UOecQuickSlotPanelWidget* GetQuickSlotPanel() const { return QuickSlotPanel; }

	class UOecInventoryPanelWidget* GetInventoryPanel() const { return InventoryPanel; }

	UOecStatPanelWidget* GetStatPanel() const { return StatPanel; }
	UOecWeaponPanelWidget* GetWeaponPanel() const { return WeaponPanel; }

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UOecQuickSlotPanelWidget* QuickSlotPanel;

	UPROPERTY(meta = (BindWidget))
	class UOecInventoryPanelWidget* InventoryPanel;

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOecCrosshairWidget> CrosshairWidget;

	void UpdateCrosshairState(bool InbIsArmed);
	void PlayCrosshairFireAnim();

	void UpdateCrosshairAimState(bool bIsAiming);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOecStatPanelWidget> StatPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOecWeaponPanelWidget> WeaponPanel;
};
