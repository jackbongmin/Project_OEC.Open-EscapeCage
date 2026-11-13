// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OecHUD.generated.h"

/**
 * 
 */
class UOecInGameWidget;

UCLASS()
class OEC_API AOecHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "OEC|UI")
	FORCEINLINE UOecInGameWidget* GetInGameWidget() const { return InGameWidget; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|UI")
	TSubclassOf<UOecInGameWidget> InGameWidgetClass;

	UPROPERTY()
	UOecInGameWidget* InGameWidget;
};
