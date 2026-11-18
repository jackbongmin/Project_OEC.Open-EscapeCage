// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/OecEnumType.h"
#include "Data/OecDataStruct.h"
#include "OecStageManagerSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStackChangedDelegate, int32, InCurrentStack, int32, InMaxStack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyRewardedDelegate, int32, InKeyCount);
/**
 * 
 */
UCLASS()
class OEC_API UOecStageManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;
	virtual void Deinitialize() override;

	// 스택 보상 (페이즈 1: 전구, 페이즈 2: 열쇠 아이템 투하)
	UFUNCTION(BlueprintCallable, Category = "OEC|Stage")
	void AddStack(int32 InAmount);

	// 열쇠 아이템을 투하구에서 떨어뜨린다 (음수면 인벤토리에서 제거)
	UFUNCTION(BlueprintCallable, Category = "OEC|Stage")
	void GrantKeys(int32 InAmount);

	UFUNCTION(BlueprintCallable, Category = "OEC|Stage")
	void ChangePhase(EOecGamePhase InNewPhase);

	// 데이터 Getter
	UFUNCTION(BlueprintCallable, Category = "OEC|Stage")
	int GetCurrentStack() const { return CurrentStack; }

	UFUNCTION(BlueprintCallable, Category = "OEC|Stage")
	int32 GetMaxStack() const { return MaxStack; }

	// 복도에 배치할 방 8개를 랜덤으로 섞어서 생성하는 함수
	UFUNCTION(BlueprintCallable, Category = "OEC|Stage")
	void GeneratePhase2Rooms(int32 InCorridorLevel, const TArray<FTransform>& InDoorTransforms);

private:
	UFUNCTION()
	void OnRunStarted(int32 InRunCount);

	// 실제 맵을 월드에 스폰하는 헬퍼 함수
	void SpawnRoomLevel(TSoftObjectPtr<UWorld> InRoomLevel, FTransform InSpawnTransform);

public:
	UFUNCTION(BlueprintCallable, Category = "OEC|Stage")
	EOecGamePhase GetCurrentPhase() const { return CurrentPhase; }

	// UI업데이트용 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnStackChangedDelegate OnStackChanged;

	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnPhaseChangedDelegate OnPhaseChanged;

	// 열쇠를 지급했을 때 (스택 보정 카운트, 연출)
	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnKeyRewardedDelegate OnKeyRewarded;

private:

	int32 CurrentStack;

	int32 MaxStack;

	EOecGamePhase CurrentPhase;


};

