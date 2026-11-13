// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/OecChoiceSpawnPoint.h"
#include "Subsystems/OecSpawnPointSubsystem.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"

AOecChoiceSpawnPoint::AOecChoiceSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	if (ArrowComponent)
	{
		ArrowComponent->SetupAttachment(RootComponent);
	}

	BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	if (BillboardComponent)
	{
		BillboardComponent->SetupAttachment(RootComponent);
	}
#endif
}

void AOecChoiceSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (UOecSpawnPointSubsystem* spawnPointSubsystem = GetWorld()->GetSubsystem<UOecSpawnPointSubsystem>())
	{
		spawnPointSubsystem->RegisterSpawnPoint(this);
	}
}

void AOecChoiceSpawnPoint::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	if (UOecSpawnPointSubsystem* spawnPointSubsystem = GetWorld()->GetSubsystem<UOecSpawnPointSubsystem>())
	{
		spawnPointSubsystem->UnregisterSpawnPoint(this);
	}

	Super::EndPlay(InEndPlayReason);
}

FTransform AOecChoiceSpawnPoint::GetRandomSpawnTransform() const
{
	const FVector2D randomOffset = FMath::RandPointInCircle(SpawnRadius);
	const FVector spawnLocation = GetActorLocation() + FVector(randomOffset.X, randomOffset.Y, 0.0f);

	return FTransform(GetActorRotation(), spawnLocation);
}
