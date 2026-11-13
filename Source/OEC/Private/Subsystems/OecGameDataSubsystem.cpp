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

}

const FQuestionStaticData* UOecGameDataSubsystem::GetQuestionData(FName QuestionID) const
{
	return CachedQuestionData.Find(QuestionID);
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

