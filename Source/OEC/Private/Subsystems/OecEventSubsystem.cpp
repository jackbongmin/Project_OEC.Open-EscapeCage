// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecEventSubsystem.h"

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
