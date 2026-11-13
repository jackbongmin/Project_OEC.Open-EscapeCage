// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OecHUD.h"
#include "UI/Ingame/OecQuickSlotPanelWidget.h"
#include "UI/Ingame/OecInGameWidget.h"
#include "Blueprint/UserWidget.h"

void AOecHUD::BeginPlay()
{
    Super::BeginPlay();

	if (InGameWidgetClass)
	{
		InGameWidget = CreateWidget<UOecInGameWidget>(GetWorld(), InGameWidgetClass);
		if (InGameWidget)
		{
			InGameWidget->AddToViewport();
		}
	}
}
