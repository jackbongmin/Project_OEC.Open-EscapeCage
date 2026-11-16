// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interfaces/OecPoolableInterface.h"
#include "OecPoolingSubsystem.generated.h"

// 클래스별 대기 액터 목록 (TMap 값에 TArray를 바로 UPROPERTY로 못 넣어서 구조체로 감쌈)
USTRUCT()
struct FOecActorPoolList
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AActor>> Actors;
};

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

	UFUNCTION(BlueprintCallable, Category = "OEC|Pooling")
	AActor* SpawnPooledActor(TSubclassOf<AActor> InClassToSpawn, const FTransform& InSpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "OEC|Pooling")
	void ReturnToPool(AActor* InActorToReturn);

	UFUNCTION(BlueprintCallable, Category = "OEC|Pooling")
	void InitializePool(TSubclassOf<AActor> InClassToInit, int32 InCount);

	UFUNCTION(BlueprintCallable, Category = "OEC|Pooling")
	void ReturnAllActiveActors();

	UFUNCTION(BlueprintCallable, Category = "OEC|Pooling")
	void ClearAllPools();

private:
	// 비활성화 후 대기 목록에 넣음 (이미 대기 중이면 무시)
	void DeactivateAndStore(AActor* InActor);

private:
	// UPROPERTY로 둬야 액터가 파괴됐을 때 참조가 정리되고 IsValid 검사가 안전함
	UPROPERTY()
	TMap<TObjectPtr<UClass>, FOecActorPoolList> ActorPool;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> ActiveActors;

};
