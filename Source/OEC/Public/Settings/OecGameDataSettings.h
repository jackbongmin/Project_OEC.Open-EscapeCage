// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "OecGameDataSettings.generated.h"

class UGameplayEffect;
class AOecItemPickupBase;

/**
 * 프로젝트 설정 > Game > OEC Game Data Settings
 * 데이터 테이블, 게임 규칙 수치, 시스템이 쓰는 기본 에셋을 등록한다.
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "OEC Game Data Settings"))
class OEC_API UOecGameDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> QuestionDataTable;

    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> RoomDataTable;

    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> ItemDataTable;

    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> MonsterDataTable;

    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> SanityDataTable;

    /* ==========================================
           스테이지 규칙
        ========================================== */
    // 페이즈 1 전구 개수 (다 켜지면 페이즈 2로 전환)
    UPROPERTY(Config, EditAnywhere, Category = "Stage", meta = (ClampMin = "1"))
    int32 Phase1MaxStack = 5;

    /* ==========================================
           선택지 효과
        ========================================== */
    // 체력 증감 GE (Instant, Health 모디파이어를 SetByCaller "Effect.Stat.Health"로)
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TSoftClassPtr<UGameplayEffect> ChoiceHealthEffect;

    // 정신력 증감 GE (Instant, Sanity 모디파이어를 SetByCaller "Effect.Stat.Sanity"로)
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TSoftClassPtr<UGameplayEffect> ChoiceSanityEffect;

    // 버프 ID → 버프 GE (Effect.Buff의 Param으로 찾음)
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TMap<FName, TSoftClassPtr<UGameplayEffect>> ChoiceBuffEffects;

    // 아이템 데이터에 픽업 액터가 없을 때 쓰는 기본 픽업
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TSoftClassPtr<AOecItemPickupBase> DefaultPickupClass;

};
