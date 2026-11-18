// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecStageManagerSubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Subsystems/OecRunSubsystem.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Choice/OecChoiceSubsystem.h"
#include "GAS/OecGameplayTags.h"
#include "Settings/OecGameDataSettings.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Algo/RandomShuffle.h"

void UOecStageManagerSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	CurrentStack = 0;
	MaxStack = GetDefault<UOecGameDataSettings>()->Phase1MaxStack;
	CurrentPhase = EOecGamePhase::Phase1_MainRoom;

	if (UOecRunSubsystem* runSubsystem = InCollection.InitializeDependency<UOecRunSubsystem>())
	{
		runSubsystem->OnRunStarted.AddDynamic(this, &UOecStageManagerSubsystem::OnRunStarted);
	}
}

void UOecStageManagerSubsystem::OnRunStarted(int32 InRunCount)
{
	// 새 회차는 메인 방(페이즈 1), 전구 0개부터
	CurrentStack = 0;
	CurrentPhase = EOecGamePhase::Phase1_MainRoom;
	OnStackChanged.Broadcast(CurrentStack, MaxStack);
	OnPhaseChanged.Broadcast();
}

void UOecStageManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UOecStageManagerSubsystem::AddStack(int32 InAmount)
{
	// 페이즈 2: 스택 대신 열쇠 아이템
	if (CurrentPhase != EOecGamePhase::Phase1_MainRoom)
	{
		GrantKeys(InAmount);
		return;
	}

	CurrentStack = FMath::Clamp(CurrentStack + InAmount, 0, MaxStack);
	OnStackChanged.Broadcast(CurrentStack, MaxStack);

	// 전구가 다 켜지면 페이즈 2로 넘어가고 메인 문 열쇠를 떨어뜨린다 (문은 플레이어가 직접 연다)
	if (CurrentStack >= MaxStack)
	{
		UE_LOG(LogTemp, Warning, TEXT("페이즈 2 진입"));
		ChangePhase(EOecGamePhase::Phase2_Corridor);
		GrantKeys(GetDefault<UOecGameDataSettings>()->Phase1CompleteKeyCount);
	}
}

void UOecStageManagerSubsystem::GrantKeys(int32 InAmount)
{
	if (InAmount == 0) return;

	const FName keyItemCode = GetDefault<UOecGameDataSettings>()->KeyItemCode;

	// 잃는 경우: 가지고 있는 열쇠에서 제거
	if (InAmount < 0)
	{
		if (UOecInventorySubsystem* inventorySubsystem = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>())
		{
			inventorySubsystem->RemoveItem(keyItemCode, -InAmount);
		}
		return;
	}

	// 얻는 경우: 아이템 지급 효과로 투하구에서 떨어뜨린다
	if (UOecChoiceSubsystem* choiceSubsystem = GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>())
	{
		FOecChoiceEffect keyEffect;
		keyEffect.EffectTag = OecGameplayTags::Effect_Give_Item;
		keyEffect.Value = InAmount;
		keyEffect.Param = keyItemCode;
		choiceSubsystem->ExecuteEffect(keyEffect, nullptr);
	}

	UE_LOG(LogTemp, Log, TEXT("열쇠 %d개 지급"), InAmount);
	OnKeyRewarded.Broadcast(InAmount);
}

void UOecStageManagerSubsystem::ChangePhase(EOecGamePhase InNewPhase)
{
	CurrentPhase = InNewPhase;

	OnPhaseChanged.Broadcast();

}

void UOecStageManagerSubsystem::GeneratePhase2Rooms(int32 InCorridorLevel, const TArray<FTransform>& InDoorTransforms)
{
    if (InDoorTransforms.Num() != 8)
    {
        UE_LOG(LogTemp, Warning, TEXT("문 좌표가 8개가 아닙니다! 현재 개수: %d"), InDoorTransforms.Num());
        return;
    }

    UOecGameDataSubsystem* dataSubsystem = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
    if (!dataSubsystem) return;

    TArray<FName> roomPool;

    // 복도 레벨에 따라 섞을 방의 비율을 다르게 설정
    if (InCorridorLevel == 1)
    {
        // 복도 1: 다음 복도로 가는 문 1개 포함
        roomPool.Add(TEXT("Room_NextCorridor"));
        roomPool.Add(TEXT("Room_Danger_01"));
        roomPool.Add(TEXT("Room_Danger_02"));
        roomPool.Add(TEXT("Room_Danger_03"));
        roomPool.Add(TEXT("Room_Supply_01"));
        roomPool.Add(TEXT("Room_Supply_02"));
        roomPool.Add(TEXT("Room_Safe_01"));
        roomPool.Add(TEXT("Room_Safe_02"));
    }
    else if (InCorridorLevel == 2)
    {
        // 복도 2: 진짜 출구 1개 포함, 위험 방 비율 증가
        roomPool.Add(TEXT("Room_Exit"));
        roomPool.Add(TEXT("Room_Danger_01"));
        roomPool.Add(TEXT("Room_Danger_02"));
        roomPool.Add(TEXT("Room_Danger_03"));
        roomPool.Add(TEXT("Room_Danger_04"));
        roomPool.Add(TEXT("Room_Supply_01"));
        roomPool.Add(TEXT("Room_Supply_02"));
        roomPool.Add(TEXT("Room_Safe_01"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("잘못된 복도 레벨입니다: %d"), InCorridorLevel);
        return;
    }

    Algo::RandomShuffle(roomPool);

    for (int32 i = 0; i < 8; ++i)
    {
        const FRoomStaticData* roomData = dataSubsystem->GetRoomData(roomPool[i]);
        if (roomData)
        {
            SpawnRoomLevel(roomData->RoomLevel, InDoorTransforms[i]);
            UE_LOG(LogTemp, Log, TEXT("복도 %d - %d번 문 뒤에 [%s] 방 생성 완료"), InCorridorLevel, i + 1, *roomPool[i].ToString());
        }
    }
}

void UOecStageManagerSubsystem::SpawnRoomLevel(TSoftObjectPtr<UWorld> InRoomLevel, FTransform InSpawnTransform)
{
    bool bOutSuccess = false;

    ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
        GetWorld(),
        InRoomLevel,
        InSpawnTransform.GetLocation(),
        InSpawnTransform.GetRotation().Rotator(),
        bOutSuccess
    );

    if (!bOutSuccess)
    {
        UE_LOG(LogTemp, Error, TEXT("방 스폰 실패!"));
    }
}

