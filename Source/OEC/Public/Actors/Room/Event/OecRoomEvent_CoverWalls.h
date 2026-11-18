// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEvent_RisingStructures.h"
#include "OecRoomEvent_CoverWalls.generated.h"

/**
 * 엄폐 벽: 방 안에 벽 3개가 솟음 (페이즈가 끝날 때까지 유지)
 */
UCLASS()
class OEC_API AOecRoomEvent_CoverWalls : public AOecRoomEvent_RisingStructures
{
	GENERATED_BODY()

public:
	AOecRoomEvent_CoverWalls();
};
