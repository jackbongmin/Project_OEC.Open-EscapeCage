// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecTerminalWidget.generated.h"

/**
 * 
 */

class UTextBlock;

UCLASS()
class OEC_API UOecTerminalWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "OEC|Terminal")
    void UpdateTerminalUI(const FString& InQuestion, const FString& InRedText, const FString& InBlueText);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_Question;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_RedBtn;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_BlueBtn;
};
