// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OecInteractableInterface.h"
#include "OecInteractableBase.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class OEC_API AOecInteractableBase : public AActor, public IOecInteractableInterface
{
	GENERATED_BODY()
	
public:
    AOecInteractableBase();

    UFUNCTION(BlueprintCallable, Category = "OEC|Interaction")
    virtual void SetInteractPromptVisible(bool bInVisible);

    virtual void Interact_Implementation(AActor* InInteractor) override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    UWidgetComponent* InteractPromptWidget;
};
