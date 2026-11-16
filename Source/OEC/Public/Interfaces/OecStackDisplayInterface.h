// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OecStackDisplayInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UOecStackDisplayInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 스택(전구) 한 칸을 표시하는 대상
 *
 * - AOecStackIndicator가 스택 수에 맞춰 칸마다 켜고 끈다.
 * - 전구 액터(AOecStackBulb) 외에 모니터 화면, 레버 등 다른 표시 장치도 이 인터페이스만 구현하면 표시기에 꽂을 수 있다.
 */
class OEC_API IOecStackDisplayInterface
{
	GENERATED_BODY()

public:
	// 칸 켜기/끄기. bInPlayEffect가 false면 연출 없이 상태만 맞춘다 (시작할 때 동기화 등)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OEC|Stack")
	void SetStackLit(bool bInLit, bool bInPlayEffect);

	// 몇 번째 스택에 켜지는 칸인지 (1부터. 0 = 미지정)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OEC|Stack")
	int32 GetStackOrder() const;
};
