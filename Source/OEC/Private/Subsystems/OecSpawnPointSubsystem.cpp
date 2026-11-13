// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecSpawnPointSubsystem.h"
#include "Actors/Room/OecChoiceSpawnPoint.h"

void UOecSpawnPointSubsystem::RegisterSpawnPoint(AOecChoiceSpawnPoint* InSpawnPoint)
{
	if (!IsValid(InSpawnPoint)) return;

	SpawnPoints.AddUnique(InSpawnPoint);
}

void UOecSpawnPointSubsystem::UnregisterSpawnPoint(AOecChoiceSpawnPoint* InSpawnPoint)
{
	SpawnPoints.Remove(InSpawnPoint);
}

AOecChoiceSpawnPoint* UOecSpawnPointSubsystem::GetRandomSpawnPoint(EOecSpawnPointType InSpawnType) const
{
	TArray<AOecChoiceSpawnPoint*> candidates;
	for (const TWeakObjectPtr<AOecChoiceSpawnPoint>& spawnPoint : SpawnPoints)
	{
		if (spawnPoint.IsValid() && spawnPoint->GetSpawnType() == InSpawnType)
		{
			candidates.Add(spawnPoint.Get());
		}
	}

	if (candidates.Num() == 0) return nullptr;

	return candidates[FMath::RandRange(0, candidates.Num() - 1)];
}

int32 UOecSpawnPointSubsystem::GetSpawnPointCount(EOecSpawnPointType InSpawnType) const
{
	int32 count = 0;
	for (const TWeakObjectPtr<AOecChoiceSpawnPoint>& spawnPoint : SpawnPoints)
	{
		if (spawnPoint.IsValid() && spawnPoint->GetSpawnType() == InSpawnType)
		{
			++count;
		}
	}

	return count;
}
