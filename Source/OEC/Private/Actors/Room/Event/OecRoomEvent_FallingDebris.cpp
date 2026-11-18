// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_FallingDebris.h"
#include "Actors/Room/Event/OecEventHazard.h"
#include "Actors/Room/Event/OecEventMarker.h"
#include "TimerManager.h"

AOecRoomEvent_FallingDebris::AOecRoomEvent_FallingDebris()
{
	DebrisClass = AOecEventHazard::StaticClass();
	MarkerClass = AOecEventMarker::StaticClass();
}

void AOecRoomEvent_FallingDebris::OnEventStarted()
{
	ShowWarning();
	GetWorldTimerManager().SetTimer(DropTimerHandle, this, &AOecRoomEvent_FallingDebris::ShowWarning, DropInterval, true);
}

void AOecRoomEvent_FallingDebris::ShowWarning()
{
	if (DroppedCount >= DropCount)
	{
		GetWorldTimerManager().ClearTimer(DropTimerHandle);
		return;
	}
	++DroppedCount;

	// 1. 낙하 지점: 절반 정도는 플레이어 발밑 근처, 나머지는 방 안 무작위
	FVector targetLocation = GetActorLocation();
	const AActor* player = GetTargetPlayer();
	if (player && FMath::FRand() < TargetPlayerChance)
	{
		const FVector2D offset = FMath::RandPointInCircle(120.0f);
		targetLocation = player->GetActorLocation() + FVector(offset.X, offset.Y, 0.0f);
	}
	else
	{
		const FVector2D offset = FMath::RandPointInCircle(DropRadius);
		targetLocation += FVector(offset.X, offset.Y, 0.0f);
	}

	const FVector floorLocation = FindFloorLocation(targetLocation);

	// 2. 경고 표시 → WarningTime 뒤 낙하
	if (MarkerClass)
	{
		if (AActor* marker = GetWorld()->SpawnActor<AActor>(MarkerClass, FTransform(floorLocation + FVector(0.0f, 0.0f, 1.0f))))
		{
			marker->SetLifeSpan(WarningTime + 0.6f);
		}
	}

	FTimerHandle dropHandle;
	GetWorldTimerManager().SetTimer(dropHandle, FTimerDelegate::CreateUObject(this, &AOecRoomEvent_FallingDebris::DropDebris, floorLocation), FMath::Max(WarningTime, 0.01f), false);
}

void AOecRoomEvent_FallingDebris::DropDebris(FVector InFloorLocation)
{
	if (IsEventFinished() || !DebrisClass) return;

	const FTransform spawnTransform(InFloorLocation + FVector(0.0f, 0.0f, DropHeight));
	AOecEventHazard* debris = GetWorld()->SpawnActorDeferred<AOecEventHazard>(DebrisClass, spawnTransform, this);
	if (!debris) return;

	debris->InitHazard(this, DebrisDamage, FVector(0.0f, 0.0f, -1200.0f), false, 1.0f, true);
	debris->FinishSpawning(spawnTransform);
	debris->SetLifeSpan(5.0f);
}

void AOecRoomEvent_FallingDebris::OnEventFinished(bool bInSuccess)
{
	GetWorldTimerManager().ClearTimer(DropTimerHandle);
}
