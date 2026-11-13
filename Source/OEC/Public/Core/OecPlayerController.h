// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OecPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API AOecPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AOecPlayerController();

	UFUNCTION(BlueprintCallable, Category = "OEC|UI")
	void SetUIInputMode(bool InIsUIOnly);

protected:
	virtual void BeginPlay() override;
};
