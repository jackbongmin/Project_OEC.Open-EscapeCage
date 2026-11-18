// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecGameDataSubsystem.h"
#include "Settings/OecGameDataSettings.h"
#include "UObject/ConstructorHelpers.h"

void UOecGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CacheAllData();
}

void UOecGameDataSubsystem::Deinitialize()
{
	CachedQuestionData.Empty();
	CachedRoomData.Empty();
	CachedItemData.Empty();
	CachedMonsterData.Empty();
	CachedSanityData.Empty();
	CachedRoomEventData.Empty();

	Super::Deinitialize();
}

void UOecGameDataSubsystem::CacheAllData()
{
	const UOecGameDataSettings* settings = GetDefault<UOecGameDataSettings>();
	if (!settings)
	{
		UE_LOG(LogTemp, Error, TEXT("OecGameDataSettings를 찾을 수 없습니다!"));
		return;
	}

	CacheDataTable<FQuestionStaticData>(settings->QuestionDataTable, CachedQuestionData, TEXT("QuestionData"));
	CacheDataTable<FRoomStaticData>(settings->RoomDataTable, CachedRoomData, TEXT("RoomData"));
	CacheDataTable<FItemStaticData>(settings->ItemDataTable, CachedItemData, TEXT("ItemData"));
	CacheDataTable<FMonsterStaticData>(settings->MonsterDataTable, CachedMonsterData, TEXT("MonsterData"));
	CacheDataTable<FSanityStaticData>(settings->SanityDataTable, CachedSanityData, TEXT("SanityData"));
	CacheDataTable<FOecRoomEventData>(settings->RoomEventDataTable, CachedRoomEventData, TEXT("RoomEventData"));

	// 아이템은 행 이름으로 찾으므로, 행 이름과 ItemCode 칸이 다르면 경고 (예: 행 이름 NewRow, ItemCode Weapon_FireAxe)
	for (const TPair<FName, FItemStaticData>& itemPair : CachedItemData)
	{
		if (itemPair.Value.ItemCode != itemPair.Key)
		{
			UE_LOG(LogTemp, Error, TEXT("[ItemData] 행 이름 [%s]과 ItemCode [%s]가 다릅니다. 아이템은 행 이름으로 찾으니 행 이름을 ItemCode와 같게 바꾸세요"),
				*itemPair.Key.ToString(), *itemPair.Value.ItemCode.ToString());
		}
	}

}

const FQuestionStaticData* UOecGameDataSubsystem::GetQuestionData(FName QuestionID) const
{
	return CachedQuestionData.Find(QuestionID);
}

const FOecRoomEventData* UOecGameDataSubsystem::GetRoomEventData(FName InEventID) const
{
	return CachedRoomEventData.Find(InEventID);
}

TArray<FName> UOecGameDataSubsystem::GetRoomEventIDs() const
{
	TArray<FName> eventIDs;
	CachedRoomEventData.GetKeys(eventIDs);
	return eventIDs;
}

const FRoomStaticData* UOecGameDataSubsystem::GetRoomData(FName RoomID) const
{
	return CachedRoomData.Find(RoomID);
}

const FItemStaticData* UOecGameDataSubsystem::GetItemData(FName ItemID) const
{
	return CachedItemData.Find(ItemID);
}

const FMonsterStaticData* UOecGameDataSubsystem::GetMonsterData(FName MonsterID) const
{
	return CachedMonsterData.Find(MonsterID);
}

const FSanityStaticData* UOecGameDataSubsystem::GetSanityData(FName SanityID) const
{
	return CachedSanityData.Find(SanityID);
}

TArray<FName> UOecGameDataSubsystem::GetAllMonsterCodes() const
{
	TArray<FName> monsterCodes;
	CachedMonsterData.GetKeys(monsterCodes);
	return monsterCodes;
}

