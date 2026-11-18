// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_RisingStructures.h"
#include "Components/StaticMeshComponent.h"

AOecRoomEvent_RisingStructures::AOecRoomEvent_RisingStructures()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// 내려가는 연출이 끝난 뒤 제거
	DestroyDelay = RiseTime + 0.1f;
}

UStaticMeshComponent* AOecRoomEvent_RisingStructures::AddStructureMesh(FName InName, UStaticMesh* InMesh, const FVector& InLocation, const FVector& InScale)
{
	UStaticMeshComponent* meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(InName);
	meshComponent->SetupAttachment(RootComponent);
	meshComponent->SetStaticMesh(InMesh);
	meshComponent->SetRelativeLocation(InLocation);
	meshComponent->SetRelativeScale3D(InScale);
	meshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	meshComponent->ComponentTags.Add(TEXT("OecRise"));
	return meshComponent;
}

void AOecRoomEvent_RisingStructures::OnEventStarted()
{
	// 블루프린트에서 추가한 메시도 태그로 함께 모은다
	TArray<UStaticMeshComponent*> meshComponents;
	GetComponents<UStaticMeshComponent>(meshComponents);

	for (UStaticMeshComponent* meshComponent : meshComponents)
	{
		if (!meshComponent->ComponentHasTag(TEXT("OecRise"))) continue;

		SetBasicShapeColor(meshComponent, StructureColor);
		RiseMeshes.Add(meshComponent);
		RiseTargetLocations.Add(meshComponent->GetRelativeLocation());
		meshComponent->SetRelativeLocation(meshComponent->GetRelativeLocation() - FVector(0.0f, 0.0f, RiseDepth));
	}

	StartMove(true);
}

void AOecRoomEvent_RisingStructures::OnEventFinished(bool bInSuccess)
{
	StartMove(false);
}

void AOecRoomEvent_RisingStructures::StartMove(bool bInRising)
{
	bRising = bInRising;
	MoveElapsed = 0.0f;
	SetActorTickEnabled(true);
}

void AOecRoomEvent_RisingStructures::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);

	MoveElapsed += InDeltaTime;
	const float alpha = FMath::Clamp(MoveElapsed / RiseTime, 0.0f, 1.0f);
	const float heightOffset = FMath::InterpEaseInOut(0.0f, RiseDepth, bRising ? 1.0f - alpha : alpha, 2.0f);

	for (int32 i = 0; i < RiseMeshes.Num(); ++i)
	{
		if (RiseMeshes[i])
		{
			RiseMeshes[i]->SetRelativeLocation(RiseTargetLocations[i] - FVector(0.0f, 0.0f, heightOffset));
		}
	}

	// 움직일 때만 Tick
	if (alpha >= 1.0f)
	{
		SetActorTickEnabled(false);
	}
}
