// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/OecEnumType.h"
#include "OecSpawnPointSubsystem.generated.h"

class AOecChoiceSpawnPoint;

/**
 * 스폰 지점 레지스트리 (월드 단위)
 *
 * - 스폰 지점 액터가 BeginPlay/EndPlay에서 스스로 등록·해제한다.
 * - 효과 실행기는 액터를 직접 찾지 않고 이 서브시스템에 "어디에 소환할지"만 묻는다.
 */
UCLASS()
class OEC_API UOecSpawnPointSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterSpawnPoint(AOecChoiceSpawnPoint* InSpawnPoint);
	void UnregisterSpawnPoint(AOecChoiceSpawnPoint* InSpawnPoint);

	// 해당 종류의 스폰 지점 중 하나를 무작위로 고름 (지점이 없으면 nullptr)
	UFUNCTION(BlueprintCallable, Category = "OEC|Spawn")
	AOecChoiceSpawnPoint* GetRandomSpawnPoint(EOecSpawnPointType InSpawnType) const;

	UFUNCTION(BlueprintPure, Category = "OEC|Spawn")
	int32 GetSpawnPointCount(EOecSpawnPointType InSpawnType) const;

private:
	TArray<TWeakObjectPtr<AOecChoiceSpawnPoint>> SpawnPoints;
};
