// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecStageManagerSubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Settings/OecGameDataSettings.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Algo/RandomShuffle.h"

void UOecStageManagerSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	CurrentStack = 0;
	MaxStack = GetDefault<UOecGameDataSettings>()->Phase1MaxStack;
	CurrentPhase = EOecGamePhase::Phase1_MainRoom;
}

void UOecStageManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UOecStageManagerSubsystem::AddStack(int32 InAmount)
{
	CurrentStack = FMath::Clamp(CurrentStack + InAmount, 0, MaxStack);
	OnStackChanged.Broadcast(CurrentStack, MaxStack);

	if(CurrentStack>=MaxStack && CurrentPhase == EOecGamePhase::Phase1_MainRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("페이즈 2 진입"));
		ChangePhase(EOecGamePhase::Phase2_Corridor);
	}
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

