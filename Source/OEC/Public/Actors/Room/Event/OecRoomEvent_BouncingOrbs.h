// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_BouncingOrbs.generated.h"

class AOecEventHazard;

/**
 * 튀는 구체: 에너지 구체가 벽에 튕기며 날아다니고, 닿으면 피해 (QuestionGuideline.md 5장)
 */
UCLASS()
class OEC_API AOecRoomEvent_BouncingOrbs : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_BouncingOrbs();

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	TSubclassOf<AOecEventHazard> OrbClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "1"))
	int32 OrbCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float OrbSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float OrbDamage = 5.0f;

	// 바닥에서 생성 높이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	float SpawnHeight = 120.0f;

private:
	UPROPERTY()
	TArray<TObjectPtr<AOecEventHazard>> SpawnedOrbs;
};
