// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecPoolingSubsystem.h"

AActor* UOecPoolingSubsystem::SpawnPooledActor(TSubclassOf<AActor> InClassToSpawn, const FTransform& InSpawnTransform)
{
	if (!InClassToSpawn)
	{
		return nullptr;
	}

	AActor* PooledActor = nullptr;
	if (FOecActorPoolList* PoolList = ActorPool.Find(InClassToSpawn))
	{
		while (PoolList->Actors.Num() > 0)
		{
			PooledActor = PoolList->Actors.Pop();
			if (IsValid(PooledActor))
			{
				break;
			}
		}
	}

	if (!IsValid(PooledActor))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		PooledActor = GetWorld()->SpawnActor<AActor>(InClassToSpawn, InSpawnTransform, SpawnParams);
	}

	if (PooledActor)
	{
		// 숨겨둔 위치에서 순간이동 (물리 속도가 이어지지 않게)
		PooledActor->SetActorTransform(InSpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

		ActiveActors.Add(PooledActor);

		if (PooledActor->Implements<UOecPoolableInterface>())
		{
			IOecPoolableInterface::Execute_OnPoolActivate(PooledActor);
		}
	}

	return PooledActor;
}

void UOecPoolingSubsystem::ReturnToPool(AActor* InActorToReturn)
{
	if (!IsValid(InActorToReturn))
	{
		return;
	}

	// 맵에 직접 배치한 액터는 ActiveActors에 없지만, 반납되면 그대로 풀에 넣어 재사용한다
	ActiveActors.Remove(InActorToReturn);
	DeactivateAndStore(InActorToReturn);
}

void UOecPoolingSubsystem::InitializePool(TSubclassOf<AActor> InClassToInit, int32 InCount)
{
	TArray<AActor*> CachedActors;

	for (int32 i = 0; i < InCount; ++i)
	{
		AActor* NewActor = SpawnPooledActor(InClassToInit, FTransform::Identity);
		CachedActors.Add(NewActor);
	}

	for (AActor* CachedActor : CachedActors)
	{
		ReturnToPool(CachedActor);
	}
}

void UOecPoolingSubsystem::ReturnAllActiveActors()
{
	TArray<TObjectPtr<AActor>> ActorsToReturn = ActiveActors.Array();

	ActiveActors.Empty();

	for (AActor* Actor : ActorsToReturn)
	{
		if (IsValid(Actor))
		{
			DeactivateAndStore(Actor);
		}
	}
}

void UOecPoolingSubsystem::ClearAllPools()
{
	ReturnAllActiveActors();

	for (auto& PoolPair : ActorPool)
	{
		for (AActor* PooledActor : PoolPair.Value.Actors)
		{
			if (IsValid(PooledActor))
			{
				PooledActor->Destroy();
			}
		}
	}

	ActorPool.Empty();
	ActiveActors.Empty();
}

void UOecPoolingSubsystem::DeactivateAndStore(AActor* InActor)
{
	FOecActorPoolList& PoolList = ActorPool.FindOrAdd(InActor->GetClass());

	// 같은 액터를 두 번 반납하면 나중에 한 액터가 두 곳에 동시에 소환됨
	if (PoolList.Actors.Contains(InActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Pooling] 이미 풀에 있는 액터를 다시 반납했습니다: %s"), *InActor->GetName());
		return;
	}

	if (InActor->Implements<UOecPoolableInterface>())
	{
		IOecPoolableInterface::Execute_OnPoolDeactivate(InActor);
	}

	PoolList.Actors.Add(InActor);
}
