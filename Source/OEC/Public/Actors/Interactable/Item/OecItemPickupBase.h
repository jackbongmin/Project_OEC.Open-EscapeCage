// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable/OecInteractableBase.h"
#include "OecItemPickupBase.generated.h"

class USoundBase;

/**
 * 아이템 픽업
 * - 맵에 배치하거나, 선택지 효과(Effect.Give.Item)가 투하구에서 떨어뜨린다.
 * - 떨어질 때는 물리 없이 중력 가속으로 내려와 바닥에 닿으면 착지 소리를 낸다 (움직이는 동안만 Tick).
 */
UCLASS()
class OEC_API AOecItemPickupBase : public AOecInteractableBase
{
	GENERATED_BODY()

public:
	AOecItemPickupBase();

	virtual void Tick(float InDeltaTime) override;

	virtual void Interact_Implementation(AActor* InInteractor) override;

	// 런타임에 스폰한 픽업의 아이템 지정 (선택지 효과 등)
	UFUNCTION(BlueprintCallable, Category = "OEC|Item")
	void SetItemData(FName InItemCode, int32 InQuantity);

	// 지금 위치에서 바닥 높이(InGroundZ)까지 떨어지기 시작. 닿으면 InLandSound 재생
	void StartDrop(float InGroundZ, USoundBase* InLandSound);

protected:
	// 바닥에 닿았을 때 (연출용)
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Item")
	void BP_OnLanded();

	UPROPERTY(EditAnywhere, Category = "OEC|Item")
	FName ItemCode;

	UPROPERTY(EditAnywhere, Category = "OEC|Item")
	int32 Quantity;

	// 낙하 중력 가속도 (cm/s²)
	UPROPERTY(EditAnywhere, Category = "OEC|Item", meta = (ClampMin = "100"))
	float DropGravity = 1800.0f;

private:
	UPROPERTY()
	TObjectPtr<USoundBase> PendingLandSound;

	// 착지했을 때의 액터 높이 (바운드 아래쪽이 바닥에 닿는 높이)
	float LandActorZ = 0.0f;

	float DropSpeed = 0.0f;
};
