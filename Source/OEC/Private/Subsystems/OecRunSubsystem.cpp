// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecRunSubsystem.h"
#include "Settings/OecGameDataSettings.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UOecRunSubsystem::Deinitialize()
{
	GetGameInstance()->GetTimerManager().ClearTimer(RestartTimerHandle);

	Super::Deinitialize();
}

void UOecRunSubsystem::EndRun(EOecRunEndReason InReason)
{
	if (bIsRunEnding) return;

	bIsRunEnding = true;
	LastEndReason = InReason;

	UE_LOG(LogTemp, Warning, TEXT("%d회차 종료 (사유 %d)"), RunCount, static_cast<int32>(InReason));
	OnRunEnded.Broadcast(InReason, RunCount);

	// 자동 재시작을 켰을 때만 타이머 (레벨이 바뀌어도 유지되도록 게임 인스턴스 타이머 사용)
	const float autoRestartDelay = GetDefault<UOecGameDataSettings>()->RunAutoRestartDelay;
	if (autoRestartDelay > 0.0f)
	{
		GetGameInstance()->GetTimerManager().SetTimer(RestartTimerHandle, this, &UOecRunSubsystem::StartNextRun, autoRestartDelay, false);
	}
}

void UOecRunSubsystem::RequestNextRun()
{
	if (!bIsRunEnding) return;

	GetGameInstance()->GetTimerManager().ClearTimer(RestartTimerHandle);
	StartNextRun();
}

void UOecRunSubsystem::StartNextRun()
{
	++RunCount;
	bIsRunEnding = false;

	// 1. 회차 단위 상태 초기화 (구독한 서브시스템들이 각자 처리)
	UE_LOG(LogTemp, Warning, TEXT("%d회차 시작"), RunCount);
	OnRunStarted.Broadcast(RunCount);

	// 2. 월드를 처음 상태로 (몬스터, 픽업, 문, 방, 플레이어 스탯)
	ReloadCurrentLevel();
}

void UOecRunSubsystem::ReloadCurrentLevel()
{
	UWorld* world = GetGameInstance()->GetWorld();
	if (!world) return;

	const FString levelName = UGameplayStatics::GetCurrentLevelName(world, true);
	UGameplayStatics::OpenLevel(world, FName(*levelName));
}
