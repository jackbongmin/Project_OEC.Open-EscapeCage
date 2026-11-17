// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Abilities/GameplayAbility.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameplayTagsClasses.h"
#include "Data/OecEnumType.h"
#include "Data/OecChoiceTypes.h"
#include "OecDataStruct.generated.h"

class AOecItemPickupBase;


/*
===========================================================================
질문 데이터
===========================================================================
*/
/** 연쇄 질문의 선행 조건 1개 (이 질문에 이 버튼으로 답했어야 함) */
USTRUCT(BlueprintType)
struct FOecQuestionPrerequisite
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EOecChoiceButton Button = EOecChoiceButton::None;
};

USTRUCT(BlueprintType)
struct FQuestionStaticData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EQuestionType QuestionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString QuestionText;

    // 빨간 버튼 선택지 (문구 + 결과 + 효과)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FOecChoiceData RedChoice;

    // 파란 버튼 선택지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FOecChoiceData BlueChoice;

    /* ==========================================
           출제 조건 (QuestionGuideline.md 9장)
        ========================================== */

    // 꺼 두면 출제되지 않음 (아직 없는 콘텐츠를 쓰는 질문)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    bool bEnabled = true;

    // 출제 가능한 페이즈 번호 (1, 2). 비어 있으면 모든 페이즈
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    TArray<int32> AllowedPhases;

    // 선행 조건. 여러 개면 하나만 맞아도 됨. 비어 있으면 조건 없음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    TArray<FOecQuestionPrerequisite> RequiredChoices;

    // 이 회차부터 출제 (0 = 제한 없음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0"))
    int32 MinRunCount = 0;

    // 0보다 크면 무작위 출제보다 먼저, 작은 번호부터 순서대로 출제 (튜토리얼)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0"))
    int32 FixedOrder = 0;

    // 검수용 메모 (두 선택지의 점수 계산). 게임에서는 쓰지 않음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FString BalanceNote;
};

/*
===========================================================================
방 타입 데이터
===========================================================================
*/
USTRUCT(BlueprintType)
struct FRoomStaticData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERoomType RoomType;

    // 방 내부 레벨 (Level Instance나 별도 맵 경로)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UWorld> RoomLevel;

    // 방에서 발생할 수 있는 이벤트 태그 리스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer PossibleEventTags;
};

/*
===========================================================================
아이템 데이터
===========================================================================
*/
USTRUCT(BlueprintType)
struct FItemStaticData : public FTableRowBase
{
    GENERATED_BODY()

    /* ==========================================
           공통 기본 정보 (항상 보임)
        ========================================== */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    FName ItemCode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    FString ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    UTexture2D* ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    FText ItemDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    EItemType ItemType;

    // 무기면 장착할 액터, 소모품이면 월드 효과용 액터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    TSoftClassPtr<AActor> ItemActorClass;

    // 월드에 떨어질 때 쓰는 픽업 액터 (비우면 프로젝트 설정의 기본 픽업 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    TSoftClassPtr<AOecItemPickupBase> PickupActorClass;


    /* ==========================================
       무기(Weapon) 전용 수치
       (ItemType이 Weapon일 때만 에디터에 표시됨!)
    ========================================== */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    float AttackRange = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    int32 ClipSize = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    FName UseAmmoItemCode; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    UAnimMontage* FireMontage; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    UAnimMontage* ReloadMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    EOecWeaponState WeaponAnimState = EOecWeaponState::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    TSubclassOf<class UOecGameplayAbility> WeaponAbilityClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "ItemType==EItemType::Weapon", EditConditionHides))
    TSubclassOf<class UOecGameplayAbility> ReloadAbilityClass;

    /* ==========================================
       소모품(Consumable) 전용 수치
       (ItemType이 Consumable일 때만 에디터에 표시됨!)
    ========================================== */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (EditCondition = "ItemType==EItemType::Consumable", EditConditionHides))
    float HealthRestoreAmount = 0.0f; // 체력 회복량

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (EditCondition = "ItemType==EItemType::Consumable", EditConditionHides))
    float SanityRestoreAmount = 0.0f; // 정신력 회복량

    // 향후 GAS 연동을 위한 태그 (Heal, SpeedUp 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (EditCondition = "ItemType==EItemType::Consumable", EditConditionHides))
    FGameplayTag EffectTag;

    /* ==========================================
       탄약(Ammo) 전용 수치
       (ItemType이 Ammo일 때만 에디터에 표시됨!)
    ========================================== */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (EditCondition = "ItemType==EItemType::Ammo", EditConditionHides))
    int32 AmmoGiveAmount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (EditCondition = "ItemType==EItemType::Ammo", EditConditionHides))
    FGameplayTag AmmoTag;
};

class UOecGameplayAbility;

/*
===========================================================================
몬스터 데이터
===========================================================================
*/
USTRUCT(BlueprintType)
struct FMonsterStaticData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MonsterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<APawn> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DropItemCode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAnimMontage> DeathMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AttackSocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UOecGameplayAbility>> DefaultAbilities;
};

/*
===========================================================================
정신력 데이터
===========================================================================
*/
USTRUCT(BlueprintType)
struct FSanityStaticData : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 이 데이터가 어떤 상태를 나타내는지 (핵심 식별자)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESanityState SanityState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxValue;

    // 해당 단계에 진입했을 때 부여할 버프/디버프 (GAS 적용용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayEffect> SanityEffectClass;

    // 패닉 상태 전용 (글자 도배 기믹 활성화)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsPanic;
};

/*
===========================================================================
 인벤토리 슬롯 구조체
===========================================================================
*/
USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemCode = NAME_None; // 어떤 아이템인가?

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity = 0; // 몇 개인가?
};

// 선택 기록은 Data/OecChoiceTypes.h의 FOecChoiceRecord 사용