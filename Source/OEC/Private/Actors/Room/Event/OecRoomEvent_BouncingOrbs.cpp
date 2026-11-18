// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_BouncingOrbs.h"
#include "Actors/Room/Event/OecEventHazard.h"

AOecRoomEvent_BouncingOrbs::AOecRoomEvent_BouncingOrbs()
{
	OrbClass = AOecEventHazard::StaticClass();
}

void AOecRoomEvent_BouncingOrbs::OnEventStarted()
{
	if (!OrbClass) return;

	const FVector center = GetActorLocation() + FVector(0.0f, 0.0f, SpawnHeight);
	for (int32 i = 0; i < OrbCount; ++i)
	{
		// 수평에 가까운 무작위 방향 (너무 위아래로만 튀지 않게)
		FVector direction = FMath::VRand();
		direction.Z *= 0.35f;
		direction.Normalize();

		AOecEventHazard* orb = GetWorld()->SpawnActorDeferred<AOecEventHazard>(OrbClass, FTransform(center), this);
		if (!orb) continue;

		orb->InitHazard(this, OrbDamage, direction * OrbSpeed, true, 0.0f, false);
		orb->FinishSpawning(FTransform(center));
		SpawnedOrbs.Add(orb);
	}
}

void AOecRoomEvent_BouncingOrbs::OnEventFinished(bool bInSuccess)
{
	for (AOecEventHazard* orb : SpawnedOrbs)
	{
		if (IsValid(orb))
		{
			orb->Destroy();
		}
	}
	SpawnedOrbs.Empty();
}
