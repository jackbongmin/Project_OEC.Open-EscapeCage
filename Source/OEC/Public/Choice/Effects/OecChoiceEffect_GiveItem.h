// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_GiveItem.generated.h"

/**
 * Effect.Give.Item: 아이템 지급. Param = 아이템 코드, Value = 수량
 * 아이템 투하구(AOecChoiceSpawnPoint)에 픽업 액터를 떨어뜨린다. 투하구가 없으면 인벤토리에 바로 넣는다.
 */
UCLASS()
class OEC_API UOecChoiceEffect_GiveItem : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_GiveItem();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
};
