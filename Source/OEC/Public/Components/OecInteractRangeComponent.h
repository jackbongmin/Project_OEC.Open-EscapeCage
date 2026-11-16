// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "OecInteractRangeComponent.generated.h"

/**
 * 상호작용 범위 콜리전
 *
 * - 상호작용 대상(아이템, 버튼, 문 등)에 붙인다. 플레이어(Pawn)와만 겹침(Overlap) 판정을 한다.
 * - 플레이어가 범위에 들어오면 UOecInteractionComponent가 이 컴포넌트의 주인을 후보로 등록한다.
 * - 범위 크기는 구 반경(Sphere Radius)으로 대상마다 조절한다.
 */
UCLASS(ClassGroup = (OEC), meta = (BlueprintSpawnableComponent))
class OEC_API UOecInteractRangeComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UOecInteractRangeComponent();
};
