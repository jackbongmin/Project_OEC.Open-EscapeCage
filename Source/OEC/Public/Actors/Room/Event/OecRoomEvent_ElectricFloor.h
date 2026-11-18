// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_ElectricFloor.generated.h"

class UStaticMeshComponent;
class UStaticMesh;

/**
 * 전기 바닥: 바닥 타일이 체크무늬로 번갈아 감전된다 (노란색 경고 → 파란색 감전). 감전 타일을 밟으면 피해
 */
UCLASS()
class OEC_API AOecRoomEvent_ElectricFloor : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_ElectricFloor();

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "2"))
	int32 GridSize = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "50"))
	float TileSize = 200.0f;

	// 경고(노랑) 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.1"))
	float WarningTime = 0.8f;

	// 감전(파랑) 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.1"))
	float ActiveTime = 1.7f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float ShockDamage = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float HitCooldown = 1.0f;

	// 바닥에서 이 높이 이상 떠 있으면 (점프 중) 감전되지 않음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float GroundTolerance = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor IdleColor = FLinearColor(0.12f, 0.12f, 0.14f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor WarningColor = FLinearColor(1.0f, 0.8f, 0.1f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor ActiveColor = FLinearColor(0.1f, 0.5f, 1.0f);

private:
	void BeginWarning();

	void BeginShock();

	void EndShock();

	void CheckPlayerOnShockTile();

	void SetTileColors(int32 InParity, const FLinearColor& InColor);

	int32 GetTileIndex(const FVector& InWorldLocation) const;

private:
	UPROPERTY()
	TObjectPtr<UStaticMesh> TileMesh;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> Tiles;

	// 다음에 감전될 체크무늬 (0 또는 1), 지금 감전 중인 무늬 (-1 = 없음)
	int32 NextParity = 0;

	int32 ActiveParity = -1;

	float LastHitTime = -1000.0f;

	FTimerHandle CycleTimerHandle;

	FTimerHandle CheckTimerHandle;
};
