// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecWeaponPanelWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UImage;

UCLASS()
class OEC_API UOecWeaponPanelWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
public:
	void UpdateAmmo(int32 InCurrent, int32 InReserve);
	void UpdateIcon(UTexture2D* InIcon);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CurrentAmmo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ReserveAmmo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_WeaponIcon;

};
