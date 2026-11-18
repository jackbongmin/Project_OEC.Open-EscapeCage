// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_DamageZone.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * 피해 구역 (독가스 등): 상자 영역 안에 있으면 일정 간격으로 피해. 바닥에 범위 표시
 */
UCLASS()
class OEC_API AOecRoomEvent_DamageZone : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_DamageZone();

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UBoxComponent> Zone;

	// 바닥 범위 표시 (기본 도형 평면)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> ZoneFloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float DamagePerTick = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.1"))
	float DamageInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor ZoneColor = FLinearColor(0.35f, 0.9f, 0.1f);

private:
	void ApplyZoneDamage();

	FTimerHandle DamageTimerHandle;
};
