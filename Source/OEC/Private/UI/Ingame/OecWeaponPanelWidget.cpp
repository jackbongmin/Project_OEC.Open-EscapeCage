// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecWeaponPanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UOecWeaponPanelWidget::UpdateAmmo(int32 InCurrent, int32 InReserve)
{
	if (Text_CurrentAmmo)
	{
		Text_CurrentAmmo->SetText(FText::AsNumber(InCurrent));
	}

	if (Text_ReserveAmmo)
	{
		Text_ReserveAmmo->SetText(FText::AsNumber(InReserve));
	}
}

void UOecWeaponPanelWidget::UpdateIcon(UTexture2D* InIcon)
{
	if (Img_WeaponIcon)
	{
		if (InIcon)
		{
			Img_WeaponIcon->SetBrushFromTexture(InIcon);
			Img_WeaponIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Img_WeaponIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
