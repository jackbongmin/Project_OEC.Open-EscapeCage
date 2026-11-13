// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OecInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OEC_API UOecInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UOecInteractionComponent();

    // E키 입력과 바인딩될 핵심 함수
    UFUNCTION(BlueprintCallable, Category = "OEC|Interaction")
    void TryInteract();

protected:
    // 상호작용 가능한 최대 사거리
    UPROPERTY(EditAnywhere, Category = "OEC|Interaction")
    float InteractRange = 200.f;

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY()
    class AOecInteractableBase* CurrentTarget;
};
