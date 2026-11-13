// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecEventSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "NativeGameplayTags.h"
#include "GAS/Attributeset/OecPlayerAttributeSet.h"

void UOecEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UOecEventSubsystem::ProcessQuestionResult(FName InQuestionID, FGameplayTag InTag, float InValue, FName InString)
{
	UsedQuestionIDs.AddUnique(InQuestionID);

    // TODO: GAS AnswerCount 어트리뷰트 증가 로직

	UE_LOG(LogTemp, Log, TEXT("질문 ID [%s] 처리 완료 누적 답변 횟수: %d"), *InQuestionID.ToString(), UsedQuestionIDs.Num());

	RecordChoice(InTag, InValue);

	if (InTag == FGameplayTag::RequestGameplayTag(FName("Event.Stack.Add")))
	{
		UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>();

		if (stageManager && stageManager->GetCurrentPhase() == EOecGamePhase::Phase1_MainRoom)
		{
			// 페이즈 1: 전구 스택 (소모되지 않음, 다 차면 페이즈 2로 전환)
			stageManager->AddStack((int32)InValue);
		}
		else
		{
			// 페이즈 2: 문 열쇠 스택 (닫힌 방 문을 열 때 소모)
			ModifyStacks((int32)InValue);
		}
	}

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

	UE_LOG(LogTemp, Log, TEXT("현재 스택: %d"), PlayerStacks);
}

void UOecEventSubsystem::RecordChoice(FGameplayTag InTag, float InValue)
{
	FChoiceRecord newRecord;
	newRecord.SelectedTag = InTag;
	newRecord.Value = InValue;

	ChoiceHistory.Add(newRecord);
	UE_LOG(LogTemp, Log, TEXT("현재까지 총 %d번의 선택을 기록함."), ChoiceHistory.Num());
}

void UOecEventSubsystem::HandleStatEffect(FGameplayTag InTag, float InValue)
{
	//TODO : 태그에 따라 스탯 효과 적용 로직 구현
    UE_LOG(LogTemp, Log, TEXT("스탯 효과 적용: %s, 값: %f"), *InTag.ToString(), InValue);
}

void UOecEventSubsystem::HandleSpawnMonster(FName InMonsterID)
{
	//TODO : 몬스터 ID에 따라 몬스터 스폰 로직 구현
    UE_LOG(LogTemp, Log, TEXT("몬스터 스폰 명령: %s"), *InMonsterID.ToString());
}

void UOecEventSubsystem::HandleGiveItem(FName InItemID)
{
	//TODO : 아이템 ID에 따라 아이템 지급 로직 구현
    UE_LOG(LogTemp, Log, TEXT("아이템 지급 명령: %s"), *InItemID.ToString());
}
