// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_GravityShift.generated.h"

/**
 * 중력 이상: 플레이어의 중력 배율을 바꿨다가 끝나면 되돌린다
 */
UCLASS()
class OEC_API AOecRoomEvent_GravityShift : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_GravityShift();

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.05"))
	float GravityScale = 0.35f;

private:
	float OriginalGravityScale = 1.0f;
};
