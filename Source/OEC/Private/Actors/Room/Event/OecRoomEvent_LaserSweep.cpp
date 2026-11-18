// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_LaserSweep.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

AOecRoomEvent_LaserSweep::AOecRoomEvent_LaserSweep()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RotatingRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RotatingRoot"));
	RotatingRoot->SetupAttachment(RootComponent);

	// 길이 1400, 바닥에서 35 높이 (점프로 넘을 수 있는 높이)
	BeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeamMesh"));
	BeamMesh->SetupAttachment(RotatingRoot);
	BeamMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 35.0f));
	BeamMesh->SetRelativeScale3D(FVector(14.0f, 0.12f, 0.12f));
	BeamMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (cubeMesh.Succeeded())
	{
		BeamMesh->SetStaticMesh(cubeMesh.Object);
	}

	BeamCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BeamCollision"));
	BeamCollision->SetupAttachment(RotatingRoot);
	BeamCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 35.0f));
	BeamCollision->SetBoxExtent(FVector(700.0f, 8.0f, 8.0f));
	BeamCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BeamCollision->OnComponentBeginOverlap.AddDynamic(this, &AOecRoomEvent_LaserSweep::OnBeamBeginOverlap);
}

void AOecRoomEvent_LaserSweep::OnEventStarted()
{
	SetBasicShapeColor(BeamMesh, BeamColor);
	SetActorTickEnabled(true);
}

void AOecRoomEvent_LaserSweep::OnEventFinished(bool bInSuccess)
{
	SetActorTickEnabled(false);
}

void AOecRoomEvent_LaserSweep::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);

	RotatingRoot->AddRelativeRotation(FRotator(0.0f, RotationSpeed * InDeltaTime, 0.0f));
}

void AOecRoomEvent_LaserSweep::OnBeamBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult)
{
	const APawn* pawn = Cast<APawn>(InOtherActor);
	if (!pawn || !pawn->IsPlayerControlled()) return;

	const float currentTime = GetWorld()->GetTimeSeconds();
	if (currentTime - LastHitTime < HitCooldown) return;

	LastHitTime = currentTime;
	ApplyDamageToPlayer(BeamDamage);
}
