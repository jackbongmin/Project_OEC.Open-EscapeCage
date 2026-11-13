// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OecInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOecInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OEC_API IOecInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OEC|Interaction")
	void Interact(AActor* InInteractor);
};
