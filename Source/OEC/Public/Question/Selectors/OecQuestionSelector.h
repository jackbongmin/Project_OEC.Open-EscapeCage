// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/OecDataStruct.h"
#include "Data/OecQuestionTypes.h"
#include "OecQuestionSelector.generated.h"

/**
 * 질문 선택기 (베이스)
 *
 * - 필터를 통과한 후보 중에서 질문 하나를 고르는 방법 1개 = 선택기 클래스 1개
 * - UOecQuestionSubsystem이 우선순위가 높은 선택기부터 시도하고, nullptr을 돌려주면 다음 선택기로 넘어간다.
 *   예) 고정 순서(300) → 스택 보정(200) → 타입 가중치 무작위(100)
 * - 하위 클래스를 추가하면 자동 등록된다.
 */
UCLASS(Abstract)
class OEC_API UOecQuestionSelector : public UObject
{
	GENERATED_BODY()

public:
	// 고를 질문이 없으면 nullptr (다음 선택기로 넘어감)
	virtual const FQuestionStaticData* SelectQuestion(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const PURE_VIRTUAL(UOecQuestionSelector::SelectQuestion, return nullptr;);

	int32 GetPriority() const { return Priority; }

protected:
	// 클수록 먼저 시도. 하위 클래스 생성자에서 지정
	int32 Priority = 0;
};
