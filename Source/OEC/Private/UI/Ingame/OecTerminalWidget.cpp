// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecTerminalWidget.h"
#include "Components/TextBlock.h"


void UOecTerminalWidget::UpdateTerminalUI(const FString& InQuestion, const FString& InRedText, const FString& InBlueText)
{
    if (Txt_Question) Txt_Question->SetText(FText::FromString(InQuestion));
    if (Txt_RedBtn) Txt_RedBtn->SetText(FText::FromString(InRedText));
    if (Txt_BlueBtn) Txt_BlueBtn->SetText(FText::FromString(InBlueText));
}
