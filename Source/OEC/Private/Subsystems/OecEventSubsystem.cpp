// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecEventSubsystem.h"

void UOecEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
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
