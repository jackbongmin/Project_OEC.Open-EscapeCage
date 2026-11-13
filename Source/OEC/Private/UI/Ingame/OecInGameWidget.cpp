// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecInGameWidget.h"
#include "UI/Ingame/OecQuickSlotPanelWidget.h"
#include "UI/Ingame/OecCrosshairWidget.h"

void UOecInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateCrosshairState(false);
}

void UOecInGameWidget::UpdateCrosshairState(bool InbIsArmed)
{
	if (CrosshairWidget)
	{
		CrosshairWidget->OnWeaponStateChanged(InbIsArmed);
	}
}

void UOecInGameWidget::PlayCrosshairFireAnim()
{
	if (CrosshairWidget)
	{
		CrosshairWidget->OnFireWeapon();
	}
}

void UOecInGameWidget::UpdateCrosshairAimState(bool bIsAiming)
{
	if (CrosshairWidget)
	{
		CrosshairWidget->OnAimStateChanged(bIsAiming);
	}
}
