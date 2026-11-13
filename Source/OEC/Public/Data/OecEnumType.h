// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EQuestionType : uint8
{
    Story       UMETA(DisplayName = "스토리"),
    Bonus       UMETA(DisplayName = "보너스"),
    Penalty     UMETA(DisplayName = "패널티"),
    Balance     UMETA(DisplayName = "밸런스"),
    Meaningless UMETA(DisplayName = "무의미")
};

UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Main          UMETA(DisplayName = "메인 방 (페이즈 1)"),
    Safe          UMETA(DisplayName = "안전 방"),
    Supply        UMETA(DisplayName = "보급 방"),
    Danger        UMETA(DisplayName = "위험 방 (몬스터)"),
    NextCorridor  UMETA(DisplayName = "다음 복도로 가는 방"),
    Exit          UMETA(DisplayName = "탈출 방 (게임 클리어)")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon      UMETA(DisplayName = "무기"),
    Consumable  UMETA(DisplayName = "소모품"),
    Ammo        UMETA(DisplayName = "탄약"),
    KeyItem     UMETA(DisplayName = "퀘스트아이템")
};

UENUM(BlueprintType)
enum class ESanityState : uint8
{
    Awake       UMETA(DisplayName = "각성"),
    Normal      UMETA(DisplayName = "노말"),
    Fear        UMETA(DisplayName = "두려움"),
    Terror      UMETA(DisplayName = "공포"),
    Panic       UMETA(DisplayName = "패닉")
};

UENUM(BlueprintType)
enum class EOecGamePhase : uint8
{
    Phase1_MainRoom     UMETA(DisplayName = "페이즈 1: 메인 방"),
    Phase2_Corridor     UMETA(DisplayName = "페이즈 2: 복도 탐색")
};

UENUM(BlueprintType)
enum class EDoorType : uint8
{
    MainDoor    UMETA(DisplayName = "메인 문"),
    RandomDoor  UMETA(DisplayName = "랜덤방 문")
};

UENUM(BlueprintType)
enum class EOecSpawnPointType : uint8
{
    Monster     UMETA(DisplayName = "몬스터 스폰구"),
    Item        UMETA(DisplayName = "아이템 투하구")
};

UENUM(BlueprintType)
enum class EOecWeaponState : uint8
{
    None        UMETA(DisplayName = "맨손"),
    Sword       UMETA(DisplayName = "검"),
    Pistol      UMETA(DisplayName = "권총"),
    Rifle       UMETA(DisplayName = "라이플")
};

UENUM(BlueprintType)
enum class EOecAbilityInputID : uint8
{
    None = 0,
    Confirm,
    Cancel,
    Fire,
    Aim,
    Reload,
    Sprint,
    Jump,
    ItemUse
};