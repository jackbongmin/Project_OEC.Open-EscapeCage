// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OecEventMarker.generated.h"

class UStaticMeshComponent;

/**
 * 바닥 경고 표시 (낙하 잔해의 붉은 원 등). 충돌 없음, 수명이 지나면 사라진다.
 */
UCLASS()
class OEC_API AOecEventMarker : public AActor
{
	GENERATED_BODY()

public:
	AOecEventMarker();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Marker")
	FLinearColor MarkerColor = FLinearColor(1.0f, 0.05f, 0.05f);
};
