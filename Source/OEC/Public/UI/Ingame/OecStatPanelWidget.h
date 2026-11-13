// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecStatPanelWidget.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;

UCLASS()
class OEC_API UOecStatPanelWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	

public:
	void UpdateStat(float InCurrent, float InMax, UProgressBar* InBar, UTextBlock* InText);

	void UpdateHealth(float InCurrent, float InMax);
	void UpdateStamina(float InCurrent, float InMax);
	void UpdateSanity(float InCurrent, float InMax);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Health;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Stamina;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Stamina;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Sanity;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Sanity;
};
