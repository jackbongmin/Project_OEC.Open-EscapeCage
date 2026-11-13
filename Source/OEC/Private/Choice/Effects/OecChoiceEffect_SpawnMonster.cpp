// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_SpawnMonster.h"
#include "Actors/Room/OecChoiceSpawnPoint.h"
#include "Subsystems/OecSpawnPointSubsystem.h"
#include "Subsystems/OecPoolingSubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "GAS/OecGameplayTags.h"
#include "GameFramework/Pawn.h"

namespace
{
	const FName RandomMonsterParam(TEXT("Random"));
}

UOecChoiceEffect_SpawnMonster::UOecChoiceEffect_SpawnMonster()
{
	EffectTag = OecGameplayTags::Effect_Spawn_Monster;
}

void UOecChoiceEffect_SpawnMonster::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	UWorld* world = GetContextWorld(InContext);
	UGameInstance* gameInstance = GetOwningGameInstance();
	if (!world || !gameInstance) return;

	UOecSpawnPointSubsystem* spawnPointSubsystem = world->GetSubsystem<UOecSpawnPointSubsystem>();
	UOecPoolingSubsystem* poolingSubsystem = world->GetSubsystem<UOecPoolingSubsystem>();
	UOecGameDataSubsystem* dataSubsystem = gameInstance->GetSubsystem<UOecGameDataSubsystem>();
	if (!spawnPointSubsystem || !poolingSubsystem || !dataSubsystem) return;

	const int32 spawnCount = FMath::Max(FMath::RoundToInt(InEffect.Value), 1);

	for (int32 i = 0; i < spawnCount; ++i)
	{
		// 무작위 소환이면 마리마다 종류를 새로 뽑음
		const FName monsterCode = ResolveMonsterCode(InContext, InEffect.Param);
		const FMonsterStaticData* monsterData = dataSubsystem->GetMonsterData(monsterCode);
		if (!monsterData)
		{
			UE_LOG(LogTemp, Error, TEXT("[Effect.Spawn.Monster] 몬스터 데이터가 없습니다: %s"), *monsterCode.ToString());
			return;
		}

		AOecChoiceSpawnPoint* spawnPoint = spawnPointSubsystem->GetRandomSpawnPoint(EOecSpawnPointType::Monster);
		if (!spawnPoint)
		{
			UE_LOG(LogTemp, Error, TEXT("[Effect.Spawn.Monster] 맵에 몬스터 스폰구(AOecChoiceSpawnPoint)가 없습니다"));
			return;
		}

		TSubclassOf<APawn> monsterClass = monsterData->MonsterClass.LoadSynchronous();
		APawn* monster = poolingSubsystem->Spawn<APawn>(monsterClass, spawnPoint->GetRandomSpawnTransform());
		if (!monster) continue;

		// 새로 생성된 몬스터는 AI 컨트롤러가 없을 수 있음 (풀에서 재사용된 몬스터는 이미 빙의 상태)
		if (!monster->GetController())
		{
			monster->SpawnDefaultController();
		}

		spawnPoint->BP_OnSpawned(monster);
		UE_LOG(LogTemp, Log, TEXT("몬스터 소환: %s"), *monsterData->MonsterName);
	}
}

FString UOecChoiceEffect_SpawnMonster::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	const int32 spawnCount = FMath::Max(FMath::RoundToInt(InEffect.Value), 1);

	if (InEffect.Param == NAME_None || InEffect.Param == RandomMonsterParam)
	{
		return FString::Printf(TEXT("무작위 적 %d마리 소환"), spawnCount);
	}

	FString monsterName = InEffect.Param.ToString();
	if (UGameInstance* gameInstance = GetOwningGameInstance())
	{
		if (const UOecGameDataSubsystem* dataSubsystem = gameInstance->GetSubsystem<UOecGameDataSubsystem>())
		{
			if (const FMonsterStaticData* monsterData = dataSubsystem->GetMonsterData(InEffect.Param))
			{
				monsterName = monsterData->MonsterName;
			}
		}
	}

	return FString::Printf(TEXT("%s %d마리 소환"), *monsterName, spawnCount);
}

FName UOecChoiceEffect_SpawnMonster::ResolveMonsterCode(const FOecChoiceEffectContext& InContext, FName InParam) const
{
	if (InParam != NAME_None && InParam != RandomMonsterParam) return InParam;

	UGameInstance* gameInstance = GetOwningGameInstance();
	const UOecGameDataSubsystem* dataSubsystem = gameInstance ? gameInstance->GetSubsystem<UOecGameDataSubsystem>() : nullptr;
	if (!dataSubsystem) return NAME_None;

	const TArray<FName> monsterCodes = dataSubsystem->GetAllMonsterCodes();
	return monsterCodes.Num() > 0 ? monsterCodes[FMath::RandRange(0, monsterCodes.Num() - 1)] : NAME_None;
}
