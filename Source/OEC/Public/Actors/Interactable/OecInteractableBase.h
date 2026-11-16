// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OecInteractableInterface.h"
#include "OecInteractableBase.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class UOecInteractRangeComponent;

UCLASS()
class OEC_API AOecInteractableBase : public AActor, public IOecInteractableInterface
{
	GENERATED_BODY()

public:
    AOecInteractableBase();

    UFUNCTION(BlueprintCallable, Category = "OEC|Interaction")
    virtual void SetInteractPromptVisible(bool bInVisible);

    virtual void Interact_Implementation(AActor* InInteractor) override;
    virtual bool CanInteract_Implementation(AActor* InInteractor) const override;
    virtual void SetInteractFocused_Implementation(bool bInFocused) override;
    virtual FVector GetInteractLocation_Implementation() const override;

    // 상호작용 가능 여부 직접 끄기/켜기 (연출 중 잠시 막기 등)
    UFUNCTION(BlueprintCallable, Category = "OEC|Interaction")
    void SetInteractionEnabled(bool bInEnabled);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    UWidgetComponent* InteractPromptWidget;

    // 플레이어가 이 범위에 들어오면 E 후보가 된다 (구 반경으로 크기 조절)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    TObjectPtr<UOecInteractRangeComponent> InteractRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Interaction")
    bool bInteractionEnabled = true;
};
