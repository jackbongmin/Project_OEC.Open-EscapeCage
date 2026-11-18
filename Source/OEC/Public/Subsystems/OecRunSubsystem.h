// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/OecEnumType.h"
#include "OecRunSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRunEndedDelegate, EOecRunEndReason, InReason, int32, InRunCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunStartedDelegate, int32, InRunCount);

/**
 * 회차(반복) 서브시스템
 *
 * 시뮬레이션 안에서 같은 고통을 반복한다는 설정을 게임 흐름으로 만든다.
 * 끝(사망·붕괴·탈출) → 종료 화면 → "다시 시작" (또는 설정한 시간 뒤 자동) → 회차 +1 → 회차 단위 상태 초기화 → 메인 방 레벨 다시 로드
 *
 * - 이 서브시스템은 누가 무엇을 초기화하는지 모른다. OnRunStarted를 발행하면
 *   각 서브시스템(스테이지, 인벤토리, 퀵슬롯, 선택지, 질문 ...)이 구독해서 스스로 초기화한다.
 *   → 회차마다 초기화할 시스템이 늘어도 이 클래스는 수정하지 않는다.
 * - 월드 액터(몬스터, 픽업, 문, 방)는 레벨을 다시 로드하면서 초기 상태로 돌아간다.
 * - 회차를 넘어 유지되는 것: 회차 수, 질문 풀 사용 기록(풀 소진 전까지 재출제 안 함), 전체 선택 기록
 */
UCLASS()
class OEC_API UOecRunSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	// 이번 회차를 끝낸다 (이미 끝나는 중이면 무시). 종료 화면은 OnRunEnded를 구독한 HUD가 띄운다
	UFUNCTION(BlueprintCallable, Category = "OEC|Run")
	void EndRun(EOecRunEndReason InReason);

	// 다음 회차 시작 (종료 화면의 "다시 시작" 버튼). 회차가 끝난 상태에서만 동작
	UFUNCTION(BlueprintCallable, Category = "OEC|Run")
	void RequestNextRun();

	UFUNCTION(BlueprintPure, Category = "OEC|Run")
	int32 GetRunCount() const { return RunCount; }

	UFUNCTION(BlueprintPure, Category = "OEC|Run")
	bool IsRunEnding() const { return bIsRunEnding; }

	UFUNCTION(BlueprintPure, Category = "OEC|Run")
	EOecRunEndReason GetLastEndReason() const { return LastEndReason; }

public:
	// 회차가 끝났을 때 (연출용: "다시 시작합니다." 등)
	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnRunEndedDelegate OnRunEnded;

	// 다음 회차가 시작될 때 (회차 단위 상태를 초기화할 시스템이 구독)
	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnRunStartedDelegate OnRunStarted;

private:
	void StartNextRun();

	void ReloadCurrentLevel();

private:
	// 현재 회차 (1부터)
	int32 RunCount = 1;

	bool bIsRunEnding = false;

	EOecRunEndReason LastEndReason = EOecRunEndReason::None;

	FTimerHandle RestartTimerHandle;
};
