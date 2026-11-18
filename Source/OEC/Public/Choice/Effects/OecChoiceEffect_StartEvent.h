// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_StartEvent.generated.h"

struct FOecRoomEventData;

/**
 * Effect.Event.Start: 메인 방에 이벤트 시작
 * - Param = 이벤트 ID (DT_RoomEvent 행 이름). Random이면 현재 페이즈에서 나올 수 있는 이벤트 중 무작위 (불안정한 큐브)
 * - 생성 위치: 맵에 배치한 이벤트 기준점(OecChoiceSpawnPoint, 타입 Event). 없으면 플레이어 발밑
 * - 이벤트의 내용·종료 조건·보상은 데이터와 이벤트 액터가 정한다
 */
UCLASS()
class OEC_API UOecChoiceEffect_StartEvent : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_StartEvent();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
	virtual bool CanExecute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual bool IsHarmful(const FOecChoiceEffect& InEffect) const override;

private:
	// Random이면 후보 중 하나를 고른다 (없으면 NAME_None)
	FName ResolveEventID(FName InParam) const;

	const FOecRoomEventData* FindEventData(FName InEventID) const;

	int32 GetCurrentPhaseNumber() const;

	FTransform FindEventTransform(UWorld* InWorld, AActor* InPlayer) const;
};
