// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecEventSubsystem.h"
#include "Subsystems/OecRunSubsystem.h"
#include "Settings/OecGameDataSettings.h"

void UOecEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PlayerStacks = GetDefault<UOecGameDataSettings>()->InitialKeyStacks;

	if (UOecRunSubsystem* runSubsystem = Collection.InitializeDependency<UOecRunSubsystem>())
	{
		runSubsystem->OnRunStarted.AddDynamic(this, &UOecEventSubsystem::OnRunStarted);
	}
}

void UOecEventSubsystem::OnRunStarted(int32 InRunCount)
{
	// 질문 풀 사용 기록(UsedQuestionIDs)은 회차를 넘어 유지한다 (풀이 소진될 때까지 새 질문)
	PlayerStacks = GetDefault<UOecGameDataSettings>()->InitialKeyStacks;
	OnKeyStacksChanged.Broadcast(PlayerStacks);
}

void UOecEventSubsystem::MarkQuestionUsed(FName InQuestionID)
{
	if (InQuestionID == NAME_None) return;

	UsedQuestionIDs.AddUnique(InQuestionID);
	UE_LOG(LogTemp, Log, TEXT("질문 ID [%s] 사용 처리 (사용한 질문 %d개)"), *InQuestionID.ToString(), UsedQuestionIDs.Num());
}

bool UOecEventSubsystem::IsQuestionUsed(FName InQuestionID) const
{
	return UsedQuestionIDs.Contains(InQuestionID);
}

void UOecEventSubsystem::ResetUsedQuestions()
{
	UsedQuestionIDs.Empty();
	UE_LOG(LogTemp, Log, TEXT("모든 질문을 소진하여 리스트를 리셋합니다"));
}

void UOecEventSubsystem::ModifyStacks(int32 InAmount)
{
	PlayerStacks += InAmount;

	if (PlayerStacks < 0) PlayerStacks = 0;

	UE_LOG(LogTemp, Log, TEXT("현재 열쇠 스택: %d"), PlayerStacks);
	OnKeyStacksChanged.Broadcast(PlayerStacks);
}
