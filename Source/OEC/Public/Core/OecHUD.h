// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Data/OecEnumType.h"
#include "OecHUD.generated.h"

/**
 * 
 */
class UOecInGameWidget;
class UOecRunEndWidget;

UCLASS()
class OEC_API AOecHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "OEC|UI")
	FORCEINLINE UOecInGameWidget* GetInGameWidget() const { return InGameWidget; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|UI")
	TSubclassOf<UOecInGameWidget> InGameWidgetClass;

	UPROPERTY()
	UOecInGameWidget* InGameWidget;

	// 회차 종료 화면 (비우면 C++ 기본 화면 사용)
	UPROPERTY(EditDefaultsOnly, Category = "OEC|UI")
	TSubclassOf<UOecRunEndWidget> RunEndWidgetClass;

	UPROPERTY()
	TObjectPtr<UOecRunEndWidget> RunEndWidget;

	UFUNCTION()
	void OnRunEnded(EOecRunEndReason InReason, int32 InRunCount);
};
