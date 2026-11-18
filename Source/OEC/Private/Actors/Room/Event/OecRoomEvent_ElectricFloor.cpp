// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_ElectricFloor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

AOecRoomEvent_ElectricFloor::AOecRoomEvent_ElectricFloor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> planeMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (planeMesh.Succeeded())
	{
		TileMesh = planeMesh.Object;
	}
}

void AOecRoomEvent_ElectricFloor::OnEventStarted()
{
	// 1. 이벤트 기준점을 가운데로 GridSize x GridSize 타일 생성 (평면 메시 100 단위 기준)
	const float halfExtent = GridSize * TileSize * 0.5f;
	for (int32 row = 0; row < GridSize; ++row)
	{
		for (int32 column = 0; column < GridSize; ++column)
		{
			UStaticMeshComponent* tile = NewObject<UStaticMeshComponent>(this);
			tile->SetStaticMesh(TileMesh);
			tile->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			tile->SetupAttachment(RootComponent);
			tile->SetRelativeLocation(FVector(-halfExtent + (column + 0.5f) * TileSize, -halfExtent + (row + 0.5f) * TileSize, 2.0f));
			tile->SetRelativeScale3D(FVector(TileSize / 100.0f * 0.94f, TileSize / 100.0f * 0.94f, 1.0f));
			tile->RegisterComponent();
			SetBasicShapeColor(tile, IdleColor);
			Tiles.Add(tile);
		}
	}

	// 2. 경고 → 감전 → 해제를 무늬를 바꿔 가며 반복, 플레이어 위치는 짧은 간격으로 검사
	BeginWarning();
	GetWorldTimerManager().SetTimer(CheckTimerHandle, this, &AOecRoomEvent_ElectricFloor::CheckPlayerOnShockTile, 0.15f, true);
}

void AOecRoomEvent_ElectricFloor::BeginWarning()
{
	SetTileColors(NextParity, WarningColor);
	GetWorldTimerManager().SetTimer(CycleTimerHandle, this, &AOecRoomEvent_ElectricFloor::BeginShock, WarningTime, false);
}

void AOecRoomEvent_ElectricFloor::BeginShock()
{
	ActiveParity = NextParity;
	SetTileColors(ActiveParity, ActiveColor);
	GetWorldTimerManager().SetTimer(CycleTimerHandle, this, &AOecRoomEvent_ElectricFloor::EndShock, ActiveTime, false);
}

void AOecRoomEvent_ElectricFloor::EndShock()
{
	SetTileColors(ActiveParity, IdleColor);
	ActiveParity = -1;
	NextParity = 1 - NextParity;
	BeginWarning();
}

void AOecRoomEvent_ElectricFloor::SetTileColors(int32 InParity, const FLinearColor& InColor)
{
	for (int32 i = 0; i < Tiles.Num(); ++i)
	{
		if (((i / GridSize) + (i % GridSize)) % 2 == InParity)
		{
			SetBasicShapeColor(Tiles[i], InColor);
		}
	}
}

int32 AOecRoomEvent_ElectricFloor::GetTileIndex(const FVector& InWorldLocation) const
{
	const FVector localLocation = GetActorTransform().InverseTransformPosition(InWorldLocation);
	const float halfExtent = GridSize * TileSize * 0.5f;
	const int32 column = FMath::FloorToInt((localLocation.X + halfExtent) / TileSize);
	const int32 row = FMath::FloorToInt((localLocation.Y + halfExtent) / TileSize);

	if (column < 0 || row < 0 || column >= GridSize || row >= GridSize) return INDEX_NONE;
	return row * GridSize + column;
}

void AOecRoomEvent_ElectricFloor::CheckPlayerOnShockTile()
{
	const AActor* player = GetTargetPlayer();
	if (!player || ActiveParity < 0) return;

	// 점프 중이면 감전되지 않음 (캡슐 중심 높이 기준)
	const FVector playerLocation = player->GetActorLocation();
	if (playerLocation.Z - GetActorLocation().Z > GroundTolerance) return;

	const int32 tileIndex = GetTileIndex(playerLocation);
	if (tileIndex == INDEX_NONE || ((tileIndex / GridSize) + (tileIndex % GridSize)) % 2 != ActiveParity) return;

	const float currentTime = GetWorld()->GetTimeSeconds();
	if (currentTime - LastHitTime < HitCooldown) return;

	LastHitTime = currentTime;
	ApplyDamageToPlayer(ShockDamage);
}

void AOecRoomEvent_ElectricFloor::OnEventFinished(bool bInSuccess)
{
	GetWorldTimerManager().ClearTimer(CycleTimerHandle);
	GetWorldTimerManager().ClearTimer(CheckTimerHandle);
}
