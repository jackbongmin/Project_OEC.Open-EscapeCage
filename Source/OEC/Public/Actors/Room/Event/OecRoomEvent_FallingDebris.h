// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_FallingDebris.generated.h"

class AOecEventHazard;
class AOecEventMarker;

/**
 * 낙하 잔해: 바닥에 붉은 원 경고 → 잠시 뒤 천장에서 잔해가 떨어짐. 맞으면 피해
 */
UCLASS()
class OEC_API AOecRoomEvent_FallingDebris : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_FallingDebris();

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	TSubclassOf<AOecEventHazard> DebrisClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	TSubclassOf<AOecEventMarker> MarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "1"))
	int32 DropCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.1"))
	float DropInterval = 1.3f;

	// 경고 표시 후 낙하까지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float WarningTime = 1.0f;

	// 이벤트 중심에서 낙하 지점이 흩어지는 반경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float DropRadius = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float DropHeight = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float DebrisDamage = 10.0f;

	// 플레이어 근처에 떨어질 확률 (0이면 완전 무작위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0", ClampMax = "1"))
	float TargetPlayerChance = 0.5f;

private:
	void ScheduleNextDrop();

	void ShowWarning();

	void DropDebris(FVector InFloorLocation);

private:
	int32 DroppedCount = 0;

	FTimerHandle DropTimerHandle;
};
