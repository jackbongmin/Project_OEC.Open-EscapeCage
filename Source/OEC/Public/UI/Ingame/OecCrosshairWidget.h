// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecCrosshairWidget.generated.h"

/**
 * 
 */

class UImage;
class UCanvasPanel;
class UWidgetAnimation;


UCLASS()
class OEC_API UOecCrosshairWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
protected:
	// ==========================================
	// UI 컴포넌트 바인딩
	// ==========================================
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DotImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CrossCanvas;

	// ==========================================
	// 애니메이션 바인딩
	// ==========================================
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Anim_Fire;

public:
	void OnWeaponStateChanged(bool bIsArmed);
	void OnFireWeapon();

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Anim_Aim;

public:
	void OnAimStateChanged(bool bIsAiming);

protected:
	bool bIsCurrentlyAiming = false;
};
