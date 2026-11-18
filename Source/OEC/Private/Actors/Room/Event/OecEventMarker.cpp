// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecEventMarker.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AOecEventMarker::AOecEventMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(1.6f, 1.6f, 0.02f));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> cylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (cylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(cylinderMesh.Object);
	}
}

void AOecEventMarker::BeginPlay()
{
	Super::BeginPlay();

	AOecRoomEventBase::SetBasicShapeColor(Mesh, MarkerColor);
}
