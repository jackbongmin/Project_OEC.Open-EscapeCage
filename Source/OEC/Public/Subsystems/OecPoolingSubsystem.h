// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interfaces/OecPoolableInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "OecPoolingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecPoolingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	template<typename T>
	T* Spawn(TSubclassOf<AActor> InClassToSpawn, const FTransform& InSpawnTransform)
	{
		return Cast<T>(SpawnPooledActor(InClassToSpawn, InSpawnTransform));
	}

	UFUNCTION(BlueprintCallable)
	AActor* SpawnPooledActor(TSubclassOf<AActor> InClassToSpawn, const FTransform& InSpawnTransform);

	UFUNCTION(BlueprintCallable)
	void ReturnToPool(AActor* InActorToReturn);

	UFUNCTION(BlueprintCallable)
	void InitializePool(TSubclassOf<AActor> InClassToInit, int32 InCount);

	UFUNCTION(BlueprintCallable)
	void ReturnAllActiveActors();

	UFUNCTION(BlueprintCallable)
	void ClearAllPools();

private:
	TMap<UClass*, TArray<AActor*>> ActorPool;

	UPROPERTY()
	TSet<AActor*> ActiveActors;
	
};
