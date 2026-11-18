// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEvent_RisingStructures.h"
#include "OecRoomEvent_RisingPillar.generated.h"

/**
 * 피난 기둥: 바닥에서 기둥이 솟음. 위에 올라가면 몬스터가 닿지 못함 (몬스터 소환과 같이 쓰는 이벤트)
 */
UCLASS()
class OEC_API AOecRoomEvent_RisingPillar : public AOecRoomEvent_RisingStructures
{
	GENERATED_BODY()

public:
	AOecRoomEvent_RisingPillar();
};
