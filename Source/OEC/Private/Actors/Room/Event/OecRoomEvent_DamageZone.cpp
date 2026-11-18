// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_DamageZone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

AOecRoomEvent_DamageZone::AOecRoomEvent_DamageZone()
{
	// 이벤트 기준점에서 한쪽으로 치우친 방 절반 크기 (레벨에 맞춰 블루프린트에서 조절)
	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	Zone->SetupAttachment(RootComponent);
	Zone->SetBoxExtent(FVector(500.0f, 300.0f, 200.0f));
	Zone->SetRelativeLocation(FVector(0.0f, 300.0f, 200.0f));
	Zone->SetCollisionProfileName(TEXT("Trigger"));

	ZoneFloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneFloorMesh"));
	ZoneFloorMesh->SetupAttachment(RootComponent);
	ZoneFloorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ZoneFloorMesh->SetRelativeLocation(FVector(0.0f, 300.0f, 2.0f));
	ZoneFloorMesh->SetRelativeScale3D(FVector(10.0f, 6.0f, 1.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> planeMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (planeMesh.Succeeded())
	{
		ZoneFloorMesh->SetStaticMesh(planeMesh.Object);
	}
}

void AOecRoomEvent_DamageZone::OnEventStarted()
{
	SetBasicShapeColor(ZoneFloorMesh, ZoneColor);
	GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &AOecRoomEvent_DamageZone::ApplyZoneDamage, DamageInterval, true);
}

void AOecRoomEvent_DamageZone::ApplyZoneDamage()
{
	TArray<AActor*> overlappingPawns;
	Zone->GetOverlappingActors(overlappingPawns, APawn::StaticClass());

	for (AActor* actor : overlappingPawns)
	{
		const APawn* pawn = Cast<APawn>(actor);
		if (pawn && pawn->IsPlayerControlled())
		{
			ApplyDamageToPlayer(DamagePerTick);
		}
	}
}

void AOecRoomEvent_DamageZone::OnEventFinished(bool bInSuccess)
{
	GetWorldTimerManager().ClearTimer(DamageTimerHandle);
}
