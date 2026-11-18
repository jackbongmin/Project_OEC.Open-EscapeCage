// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/OecEnumType.h"
#include "OecChoiceSpawnPoint.generated.h"

class UArrowComponent;
class UBillboardComponent;

/**
 * 선택지 결과 스폰 지점 (메인 방 천장의 몬스터 스폰구 / 아이템 투하구)
 *
 * - 맵에 배치하고 SpawnType만 지정하면 된다. 여러 개 배치하면 무작위로 하나가 골라진다.
 * - SpawnRadius 안에서 위치가 흩어진다 (여러 마리 소환 시 겹침 방지).
 * - 연출(해치 열림, 이펙트)은 BP_OnSpawned에서 블루프린트로 처리한다.
 */
UCLASS()
class OEC_API AOecChoiceSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	AOecChoiceSpawnPoint();

	UFUNCTION(BlueprintPure, Category = "OEC|Spawn")
	EOecSpawnPointType GetSpawnType() const { return SpawnType; }

	// SpawnRadius 안의 무작위 위치 (회전은 이 액터 기준)
	FTransform GetRandomSpawnTransform() const;

	// 무작위 위치에서 바로 아래 바닥 위치 (바닥을 못 찾으면 bOutFoundGround = false)
	FTransform GetRandomGroundTransform(bool& bOutFoundGround) const;

	// 주어진 위치 바로 아래 바닥 (고정 지형만). 못 찾으면 false
	bool FindGroundBelow(const FVector& InLocation, FVector& OutGroundLocation) const;

	// 이 지점에서 무언가 소환됐을 때 호출 (연출용)
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Spawn")
	void BP_OnSpawned(AActor* InSpawnedActor);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Spawn")
	EOecSpawnPointType SpawnType = EOecSpawnPointType::Monster;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Spawn", meta = (ClampMin = "0"))
	float SpawnRadius = 100.0f;

	// 바닥을 찾을 때 아래로 검사하는 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Spawn", meta = (ClampMin = "0"))
	float GroundTraceDistance = 3000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<USceneComponent> SceneRoot;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "OEC|Components")
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(VisibleAnywhere, Category = "OEC|Components")
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif
};
