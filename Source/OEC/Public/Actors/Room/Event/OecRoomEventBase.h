// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/OecDataStruct.h"
#include "OecRoomEventBase.generated.h"

class UMeshComponent;

/**
 * 방 이벤트 (베이스)
 *
 * 질문 결과(Effect.Event.Start)로 메인 방에 생성되는 이벤트 1개 = 이 클래스의 하위 클래스 1개.
 * - 흐름: StartEvent → (시간 / 답변 수 / 페이즈 전환으로) 종료 → 성공·실패 효과 실행 → 제거
 * - 무엇이 언제 끝나고 무엇을 주는지는 데이터(FOecRoomEventData, DT_RoomEvent)가 정한다.
 *   하위 클래스는 "방에서 무슨 일이 일어나는지"만 구현한다 (구체 생성, 잔해 낙하 ...).
 * - 보상·피해는 선택지 효과 구조(FOecChoiceEffect)를 재사용하므로 GE·클램프를 그대로 통과한다.
 * - 이 클래스를 그대로 쓰면 "시작 효과만 있는 이벤트"가 된다 (예: 정전 = 방 어두워짐 + 답변 3회).
 * - 연출(소리, 파티클, 메시 교체)은 블루프린트 하위 클래스의 BP_On... 이벤트에서 처리한다.
 */
UCLASS()
class OEC_API AOecRoomEventBase : public AActor
{
	GENERATED_BODY()

public:
	AOecRoomEventBase();

	// 이벤트 시작 (효과 실행기가 스폰 직후 호출)
	void StartEvent(FName InEventID, const FOecRoomEventData& InEventData, AActor* InTargetPlayer);

	// 조건을 달성하거나 실패했을 때 즉시 끝내기 (예: 터렛 파괴 = 성공)
	UFUNCTION(BlueprintCallable, Category = "OEC|Event")
	void CompleteEvent(bool bInSuccess);

	// 이벤트 위험물이 플레이어에게 피해를 줄 때 (체력 GE + 실패 판정 기록)
	UFUNCTION(BlueprintCallable, Category = "OEC|Event")
	void ApplyDamageToPlayer(float InDamage);

	UFUNCTION(BlueprintPure, Category = "OEC|Event")
	AActor* GetTargetPlayer() const { return TargetPlayer.Get(); }

	UFUNCTION(BlueprintPure, Category = "OEC|Event")
	FName GetEventID() const { return EventID; }

	UFUNCTION(BlueprintPure, Category = "OEC|Event")
	bool IsEventFinished() const { return bFinished; }

	// 엔진 기본 도형 머티리얼(BasicShapeMaterial)의 색 바꾸기 (블루프린트 메시가 없을 때의 기본 표시용)
	static void SetBasicShapeColor(UMeshComponent* InMesh, const FLinearColor& InColor);

	// 위치에서 바로 아래 바닥 높이 찾기 (못 찾으면 입력 위치 그대로)
	FVector FindFloorLocation(const FVector& InLocation) const;

protected:
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

	// 하위 클래스: 방에 무언가를 만든다
	virtual void OnEventStarted() {}

	// 하위 클래스: 만든 것을 정리한다
	virtual void OnEventFinished(bool bInSuccess) {}

	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Event")
	void BP_OnEventStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Event")
	void BP_OnEventFinished(bool bInSuccess);

	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Event")
	void BP_OnPlayerHit(float InDamage);

	// 종료 후 제거까지 기다리는 시간 (내려가는 연출 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float DestroyDelay = 0.0f;

private:
	UFUNCTION()
	void OnChoiceResolved(const FOecChoiceRecord& InRecord, const FOecChoiceOutcome& InOutcome);

	UFUNCTION()
	void OnPhaseChanged();

	void OnDurationElapsed();

	// 시간·답변 수·페이즈 전환으로 끝날 때 (데이터의 성공 조건으로 판정)
	void FinishByCondition();

	void FinishEvent(bool bInSuccess);

	void ExecuteEffects(const TArray<FOecChoiceEffect>& InEffects);

private:
	UPROPERTY(VisibleInstanceOnly, Category = "OEC|Event")
	FName EventID;

	UPROPERTY(VisibleInstanceOnly, Category = "OEC|Event")
	FOecRoomEventData EventData;

	TWeakObjectPtr<AActor> TargetPlayer;

	bool bStarted = false;

	bool bFinished = false;

	bool bPlayerHit = false;

	int32 RemainingAnswers = 0;

	FTimerHandle DurationTimerHandle;
};
