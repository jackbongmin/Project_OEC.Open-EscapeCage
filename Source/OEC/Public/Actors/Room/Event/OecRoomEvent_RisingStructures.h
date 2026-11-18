// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_RisingStructures.generated.h"

class UStaticMeshComponent;

/**
 * 솟아오르는 구조물 (베이스): 태그 OecRise가 붙은 메시들이 바닥에서 솟고, 끝나면 다시 내려간다
 * - 피난 기둥, 엄폐 벽이 이 클래스를 쓴다. 블루프린트 하위 클래스에서 메시를 추가하고 컴포넌트 태그 OecRise를 붙이면 같이 움직인다
 */
UCLASS()
class OEC_API AOecRoomEvent_RisingStructures : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_RisingStructures();

	virtual void Tick(float InDeltaTime) override;

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	// 하위 클래스 생성자에서 구조물 메시 추가 (태그 OecRise 자동 부여)
	UStaticMeshComponent* AddStructureMesh(FName InName, UStaticMesh* InMesh, const FVector& InLocation, const FVector& InScale);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float RiseDepth = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.1"))
	float RiseTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor StructureColor = FLinearColor(0.25f, 0.28f, 0.32f);

private:
	void StartMove(bool bInRising);

private:
	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> RiseMeshes;

	TArray<FVector> RiseTargetLocations;

	bool bRising = true;

	float MoveElapsed = 0.0f;
};
