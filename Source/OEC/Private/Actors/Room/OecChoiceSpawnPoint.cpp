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

FTransform AOecChoiceSpawnPoint::GetRandomGroundTransform(bool& bOutFoundGround) const
{
	FTransform spawnTransform = GetRandomSpawnTransform();

	FVector groundLocation;
	bOutFoundGround = FindGroundBelow(spawnTransform.GetLocation(), groundLocation);
	if (bOutFoundGround)
	{
		spawnTransform.SetLocation(groundLocation);
	}

	return spawnTransform;
}

bool AOecChoiceSpawnPoint::FindGroundBelow(const FVector& InLocation, FVector& OutGroundLocation) const
{
	const FVector traceEnd = InLocation - FVector(0.0f, 0.0f, GroundTraceDistance);

	FCollisionQueryParams queryParams(SCENE_QUERY_STAT(OecSpawnGroundTrace), false, this);
	FHitResult hitResult;

	// 바닥·벽 같은 고정 지형만 검사 (다른 픽업이나 캐릭터 위에 올라가지 않게)
	if (!GetWorld()->LineTraceSingleByObjectType(hitResult, InLocation, traceEnd, FCollisionObjectQueryParams(ECC_WorldStatic), queryParams))
	{
		return false;
	}

	OutGroundLocation = hitResult.ImpactPoint;
	return true;
}
