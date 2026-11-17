// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/OecEnumType.h"

class UOecQuestionSubsystem;
class UOecEventSubsystem;
class UOecGameDataSettings;

/**
 * 질문 1개를 뽑을 때 필터와 선택기에 넘기는 정보
 * (출제 한 번 동안만 쓰는 값 묶음이라 리플렉션 없이 일반 구조체로 둔다)
 */
struct FOecQuestionContext
{
	// 현재 페이즈 번호 (1, 2)
	int32 PhaseNumber = 1;

	// 현재 회차 (1부터 시작)
	int32 RunCount = 1;

	// 스택을 얻지 못한 연속 답변 수 (스택 보정용)
	int32 AnswersWithoutStack = 0;

	// 단말기가 넘긴 타입별 가중치
	const TMap<EQuestionType, float>* TypeWeights = nullptr;

	const UOecQuestionSubsystem* QuestionSubsystem = nullptr;

	const UOecEventSubsystem* EventSubsystem = nullptr;

	const UOecGameDataSettings* Settings = nullptr;

	float GetTypeWeight(EQuestionType InType) const
	{
		const float* weight = TypeWeights ? TypeWeights->Find(InType) : nullptr;
		return weight ? FMath::Max(*weight, 0.0f) : 0.0f;
	}
};
