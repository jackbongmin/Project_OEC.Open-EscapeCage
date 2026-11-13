// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OecPoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOecPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OEC_API IOecPoolableInterface
{
	GENERATED_BODY()

public:
	// 풀 할당할 때 진행할 로직 (상태 복구 등)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPoolActivate();

	// 풀 반환할 때 진행할 로직 (동작 정지 등)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPoolDeactivate();
};
