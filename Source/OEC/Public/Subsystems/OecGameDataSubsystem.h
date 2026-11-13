// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/OecDataStruct.h"
#include "OecGameDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 데이터 조회 함수
	const FQuestionStaticData* GetQuestionData(FName QuestionID) const;
	const FRoomStaticData* GetRoomData(FName RoomID) const;
	const FItemStaticData* GetItemData(FName ItemID) const;
	const FMonsterStaticData* GetMonsterData(FName MonsterID) const;
	const FSanityStaticData* GetSanityData(FName SanityID) const;

	// 등록된 몬스터 코드 전체 (무작위 소환용)
	TArray<FName> GetAllMonsterCodes() const;

private:
	void CacheAllData();

	// 테이블을 읽어서 TMap에 넣어주는 템플릿
	template<typename T>
	void CacheDataTable(TSoftObjectPtr<UDataTable> InTablePtr, TMap<FName, T>& OutCacheMap, const FString& InLogName);

	TMap<FName, FQuestionStaticData> CachedQuestionData;
	TMap<FName, FRoomStaticData> CachedRoomData;
	TMap<FName, FItemStaticData> CachedItemData;
	TMap<FName, FMonsterStaticData> CachedMonsterData;
	TMap<FName, FSanityStaticData> CachedSanityData;
};

template<typename T>
void UOecGameDataSubsystem::CacheDataTable(TSoftObjectPtr<UDataTable> InTablePtr, TMap<FName, T>& OutCacheMap, const FString& InLogName)
{
	if (UDataTable* table = InTablePtr.LoadSynchronous())
	{
		TArray<FName> rowNames = table->GetRowNames();
		for (const FName& rowName : rowNames)
		{
			if (T* row = table->FindRow<T>(rowName, TEXT("")))
			{
				OutCacheMap.Add(rowName, *row);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[%s] %d개 항목 캐싱 완료"), *InLogName, rowNames.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 데이터 테이블이 비어있거나 로드 실패!"), *InLogName);
	}
}
