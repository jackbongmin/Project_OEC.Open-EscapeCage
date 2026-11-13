// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "OecChoiceTypes.generated.h"

class AActor;
class UOecChoiceSubsystem;

/*
===========================================================================
 선택지 데이터 구조

 선택지(Choice)
  └─ 결과 목록(Outcomes): 가중치로 하나를 뽑음
       └─ 효과 목록(Effects): 뽑힌 결과의 효과를 순서대로 실행

 예) 50%: 스택 +1 / 50%: 체력 -30
     Outcomes[0] = { Weight 50, Effects [ Effect.Stack +1 ] }
     Outcomes[1] = { Weight 50, Effects [ Effect.Stat.Health -30 ] }
===========================================================================
*/

/** 선택지 효과 1개 */
USTRUCT(BlueprintType)
struct FOecChoiceEffect
{
	GENERATED_BODY()

public:
	// 어떤 효과인지 (Effect.* 태그)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Effect"))
	FGameplayTag EffectTag;

	// 효과 수치 (증감량, 마리 수, 확률 등. 효과마다 의미가 다름)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.0f;

	// 아이템 코드, 몬스터 코드, 버프 ID 등
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Param = NAME_None;
};

/** 선택지 결과 1개 (가중치 + 효과 목록) */
USTRUCT(BlueprintType)
struct FOecChoiceOutcome
{
	GENERATED_BODY()

public:
	// 결과끼리의 비율. 결과가 하나면 값과 상관없이 항상 선택됨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float Weight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOecChoiceEffect> Effects;
};

/** 버튼 하나에 연결되는 선택지 */
USTRUCT(BlueprintType)
struct FOecChoiceData
{
	GENERATED_BODY()

public:
	// 버튼 문구. 비워두면 효과 데이터로 자동 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayText;

	// 비어 있으면 아무 효과 없는 선택지 (무의미 질문)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOecChoiceOutcome> Outcomes;
};

/** 선택 기록 (엔딩 판정, 통계용) */
USTRUCT(BlueprintType)
struct FOecChoiceRecord
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FName QuestionID = NAME_None;

	UPROPERTY(BlueprintReadOnly)
	FString ChoiceText;

	// 뽑힌 결과 번호 (결과가 없는 선택지면 INDEX_NONE)
	UPROPERTY(BlueprintReadOnly)
	int32 OutcomeIndex = INDEX_NONE;
};

/** 답변 횟수 단위로 유지되는 효과 (지속 효과, 환경 효과) */
USTRUCT(BlueprintType)
struct FOecAnswerModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag EffectTag;

	UPROPERTY(BlueprintReadOnly)
	float Value = 0.0f;

	// 남은 답변 수 (INDEX_NONE = 영구)
	UPROPERTY(BlueprintReadOnly)
	int32 RemainingAnswers = INDEX_NONE;

	bool IsPermanent() const { return RemainingAnswers == INDEX_NONE; }
};

/** 효과 실행에 필요한 정보 */
USTRUCT(BlueprintType)
struct FOecChoiceEffectContext
{
	GENERATED_BODY()

public:
	// 버튼을 누른 플레이어
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UOecChoiceSubsystem> ChoiceSubsystem = nullptr;
};
