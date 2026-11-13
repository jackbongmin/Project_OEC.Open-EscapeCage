// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecPoolingSubsystem.h"

AActor* UOecPoolingSubsystem::SpawnPooledActor(TSubclassOf<AActor> InClassToSpawn, const FTransform& InSpawnTransform)
{
	if (!InClassToSpawn)
	{
		return nullptr;
	}

	AActor* PooledActor = nullptr;
	TArray<AActor*>* PoolStack = ActorPool.Find(InClassToSpawn);
	if (PoolStack && PoolStack->Num() > 0)
	{
		while (PoolStack->Num() > 0)
		{
			PooledActor = PoolStack->Pop();
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
		PooledActor->SetActorTransform(InSpawnTransform);

		if (PooledActor->Implements<UOecPoolableInterface>())
		{
			IOecPoolableInterface::Execute_OnPoolActivate(PooledActor);
		}

		ActiveActors.Add(PooledActor);
	}

	return PooledActor;
}

void UOecPoolingSubsystem::ReturnToPool(AActor* InActorToReturn)
{
	if (!IsValid(InActorToReturn))
	{
		return;
	}

	ActiveActors.Remove(InActorToReturn);

	if (InActorToReturn->Implements<UOecPoolableInterface>())
	{
		IOecPoolableInterface::Execute_OnPoolDeactivate(InActorToReturn);
	}

	UClass* ActorClass = InActorToReturn->GetClass();
	TArray<AActor*>& PoolStack = ActorPool.FindOrAdd(ActorClass);
	PoolStack.Add(InActorToReturn);
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
	TArray<AActor*> ActorsToReturn = ActiveActors.Array();

	ActiveActors.Empty();

	for (AActor* Actor : ActorsToReturn)
	{
		if (IsValid(Actor))
		{
			if (Actor->Implements<UOecPoolableInterface>())
			{
				IOecPoolableInterface::Execute_OnPoolDeactivate(Actor);
			}
			UClass* ActorClass = Actor->GetClass();
			ActorPool.FindOrAdd(ActorClass).Push(Actor);
		}
	}
}

void UOecPoolingSubsystem::ClearAllPools()
{
	ReturnAllActiveActors();

	for (auto& PoolPair : ActorPool)
	{
		TArray<AActor*>& PoolStack = PoolPair.Value;
		for (AActor* PooledActor : PoolStack)
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
