// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OecEventSubsystem.generated.h"

/**
 * 질문 사용 기록 서브시스템
 * - 이미 나온 질문 관리 (풀이 소진될 때까지 재출제 방지, 회차를 넘어 유지)
 * - 페이즈 2 열쇠는 스택이 아니라 인벤토리 아이템이다 (프로젝트 설정 KeyItemCode)
 * 선택지 결과 처리는 UOecChoiceSubsystem이 담당한다.
 */
UCLASS()
class OEC_API UOecEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void MarkQuestionUsed(FName InQuestionID);

	bool IsQuestionUsed(FName InQuestionID) const;

	void ResetUsedQuestions();

private:
	UPROPERTY()
	TArray<FName> UsedQuestionIDs;
};
