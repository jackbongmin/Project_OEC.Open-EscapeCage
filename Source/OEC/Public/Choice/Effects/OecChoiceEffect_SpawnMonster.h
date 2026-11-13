// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_SpawnMonster.generated.h"

/**
 * Effect.Spawn.Monster: 몬스터 소환. Param = 몬스터 코드 (Random = 무작위), Value = 마리 수
 * 몬스터 스폰구(AOecChoiceSpawnPoint)에서 풀링으로 소환한다.
 */
UCLASS()
class OEC_API UOecChoiceEffect_SpawnMonster : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_SpawnMonster();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;

private:
	FName ResolveMonsterCode(const FOecChoiceEffectContext& InContext, FName InParam) const;
};
