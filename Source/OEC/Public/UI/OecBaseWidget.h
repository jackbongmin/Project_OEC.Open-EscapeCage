// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OecBaseWidget.generated.h"

/**
 * 
 */

class USoundBase;

UCLASS()
class OEC_API UOecBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "OEC|UI")
    virtual void ShowWidget();

    UFUNCTION(BlueprintCallable, Category = "OEC|UI")
    virtual void HideWidget();

    UFUNCTION(BlueprintCallable, Category = "OEC|UI")
    virtual void PlayUISound(USoundBase* InSound);

protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeConstruct() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|UI|Sound")
    USoundBase* DefaultClickSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|UI|Sound")
    USoundBase* DefaultHoverSound;
};
