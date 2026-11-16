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
 * 상호작용 대상 (E키)
 *
 * - UOecInteractionComponent가 범위 안 후보 중 하나를 골라 포커스를 주고, E를 누르면 Interact를 호출한다.
 */
class OEC_API IOecInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OEC|Interaction")
	void Interact(AActor* InInteractor);

	// 지금 상호작용할 수 있는지 (열린 문, 잠긴 대상 등은 false)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OEC|Interaction")
	bool CanInteract(AActor* InInteractor) const;

	// 포커스(현재 E 대상) 표시 켜기/끄기
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OEC|Interaction")
	void SetInteractFocused(bool bInFocused);

	// 시선 판정에 쓰는 위치 (화면 중앙과 이 위치의 각도로 대상을 고른다)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OEC|Interaction")
	FVector GetInteractLocation() const;
};
