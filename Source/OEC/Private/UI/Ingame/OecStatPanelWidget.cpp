// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecStatPanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UOecStatPanelWidget::UpdateStat(float InCurrent, float InMax, UProgressBar* InBar, UTextBlock* InText)
{
	if (InBar && InMax > 0.f)
	{
		InBar->SetPercent(InCurrent / InMax);
	}

	if (InText)
	{
		FString StatString = FString::Printf(TEXT("%d / %d"), FMath::FloorToInt(InCurrent), FMath::FloorToInt(InMax));
		InText->SetText(FText::FromString(StatString));
	}
}

void UOecStatPanelWidget::UpdateHealth(float InCurrent, float InMax)
{
	UpdateStat(InCurrent, InMax, PB_Health, Text_Health);
}

void UOecStatPanelWidget::UpdateStamina(float InCurrent, float InMax)
{
	UpdateStat(InCurrent, InMax, PB_Stamina, Text_Stamina);
}

void UOecStatPanelWidget::UpdateSanity(float InCurrent, float InMax)
{
	UpdateStat(InCurrent, InMax, PB_Sanity, Text_Sanity);
}
