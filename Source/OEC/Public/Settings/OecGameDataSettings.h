// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "Data/OecEnumType.h"
#include "OecGameDataSettings.generated.h"

class UGameplayEffect;
class AOecItemPickupBase;
class UUserWidget;
class USoundBase;

/**
 * 프로젝트 설정 > Game > OEC Game Data Settings
 * 데이터 테이블, 게임 규칙 수치, 시스템이 쓰는 기본 에셋을 등록한다.
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "OEC Game Data Settings"))
class OEC_API UOecGameDataSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
    UOecGameDataSettings();

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

    // 방 이벤트 (행 타입 OecRoomEventData)
    UPROPERTY(Config, EditAnywhere, Category = "DataTables")
    TSoftObjectPtr<UDataTable> RoomEventDataTable;

    /* ==========================================
           스테이지 규칙
        ========================================== */
    // 페이즈 1 전구 개수 (다 켜지면 페이즈 2로 전환)
    UPROPERTY(Config, EditAnywhere, Category = "Stage", meta = (ClampMin = "1"))
    int32 Phase1MaxStack = 5;

    // 문을 여는 열쇠 아이템 코드 (페이즈 2에서는 스택 대신 이 아이템이 투하된다)
    UPROPERTY(Config, EditAnywhere, Category = "Stage")
    FName KeyItemCode = TEXT("Key_Door");

    // 페이즈 1을 끝냈을 때 주는 열쇠 수 (메인 문을 직접 열게 하기 위함)
    UPROPERTY(Config, EditAnywhere, Category = "Stage", meta = (ClampMin = "0"))
    int32 Phase1CompleteKeyCount = 1;

    // 회차가 끝난 뒤 자동으로 다음 회차를 시작하기까지의 시간 (초). 0 = 종료 화면의 "다시 시작" 버튼을 누를 때까지 대기
    UPROPERTY(Config, EditAnywhere, Category = "Stage", meta = (ClampMin = "0"))
    float RunAutoRestartDelay = 0.0f;

    /* ==========================================
           질문 출제 규칙
        ========================================== */

    // 이 횟수만큼 연속으로 스택을 못 얻으면 다음 질문에 확정 스택 선택지를 보장 (0 = 끔)
    UPROPERTY(Config, EditAnywhere, Category = "Question", meta = (ClampMin = "0"))
    int32 StackPityAnswerCount = 5;

    // 같은 타입이 연속으로 나올 수 있는 최대 횟수 (0 = 제한 없음)
    UPROPERTY(Config, EditAnywhere, Category = "Question", meta = (ClampMin = "0"))
    int32 DefaultQuestionTypeStreakLimit = 2;

    // 타입별 연속 제한 (여기에 없는 타입은 위 기본값 사용)
    UPROPERTY(Config, EditAnywhere, Category = "Question")
    TMap<EQuestionType, int32> QuestionTypeStreakLimits;

    // 회차마다 다시 출제되는 타입 (그 외 타입은 풀이 소진될 때까지 재출제하지 않음)
    UPROPERTY(Config, EditAnywhere, Category = "Question")
    TArray<EQuestionType> RunScopedQuestionTypes;

    // 답변할 때마다 바뀌는 정신력 (GDD 3.2. 음수 = 감소, 0 = 끔)
    UPROPERTY(Config, EditAnywhere, Category = "Question")
    float SanityChangePerAnswer = -1.0f;

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

    // 버프의 Value(지속 답변 수)가 0 이하일 때 쓰는 기본 지속 답변 수
    UPROPERTY(Config, EditAnywhere, Category = "Choice", meta = (ClampMin = "1"))
    int32 DefaultBuffAnswerDuration = 3;

    // 해로운 버프 ID (보험 증서가 "나쁜 결과"로 판정)
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TArray<FName> HarmfulBuffIDs;

    // 버프 ID → 버튼에 표시할 이름 (없으면 ID 그대로 표시)
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TMap<FName, FText> ChoiceBuffDisplayNames;

    // 아이템 데이터에 픽업 액터가 없을 때 쓰는 기본 픽업
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TSoftClassPtr<AOecItemPickupBase> DefaultPickupClass;

    // 투하된 아이템이 바닥에 닿을 때 소리 (아이템 데이터의 DropSound가 비어 있을 때)
    UPROPERTY(Config, EditAnywhere, Category = "Choice")
    TSoftObjectPtr<USoundBase> DefaultItemDropSound;

    /* ==========================================
           상호작용
        ========================================== */
    // 상호작용 대상의 "E" 표시 위젯 기본값 (블루프린트에서 위젯 클래스를 비워두면 이걸 사용)
    UPROPERTY(Config, EditAnywhere, Category = "Interaction")
    TSoftClassPtr<UUserWidget> DefaultInteractPromptWidget;

};
