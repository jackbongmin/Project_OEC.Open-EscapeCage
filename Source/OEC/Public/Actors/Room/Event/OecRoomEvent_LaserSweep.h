// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_LaserSweep.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * 레이저 스윕: 바닥 높이의 레이저 막대가 회전한다. 점프로 넘는다. 닿으면 피해
 */
UCLASS()
class OEC_API AOecRoomEvent_LaserSweep : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_LaserSweep();

	virtual void Tick(float InDeltaTime) override;

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UFUNCTION()
	void OnBeamBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<USceneComponent> RotatingRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> BeamMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UBoxComponent> BeamCollision;

	// 초당 회전 각도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	float RotationSpeed = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float BeamDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float HitCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor BeamColor = FLinearColor(1.0f, 0.05f, 0.05f);

private:
	float LastHitTime = -1000.0f;
};
