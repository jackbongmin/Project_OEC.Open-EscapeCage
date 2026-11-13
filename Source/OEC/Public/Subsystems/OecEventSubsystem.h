// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OecEventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyStacksChangedDelegate, int32, InCurrentStacks);

/**
 * 질문 진행 상태 서브시스템
 * - 이미 나온 질문 관리 (중복 출제 방지)
 * - 페이즈 2 문 열쇠 스택
 * 선택지 결과 처리는 UOecChoiceSubsystem이 담당한다.
 */
UCLASS()
class OEC_API UOecEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void MarkQuestionUsed(FName InQuestionID);

	bool IsQuestionUsed(FName InQuestionID) const;

	void ResetUsedQuestions();

	UFUNCTION(BlueprintCallable, Category = "OEC|Stack")
	int32 GetCurrentStacks() const { return PlayerStacks; }

	UFUNCTION(BlueprintCallable, Category = "OEC|Stack")
	void ModifyStacks(int32 InAmount);

public:
	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnKeyStacksChangedDelegate OnKeyStacksChanged;

private:
	// 페이즈 2 문 열쇠 스택 (초기값 3은 테스트용)
	UPROPERTY()
	int32 PlayerStacks = 3;

	UPROPERTY()
	TArray<FName> UsedQuestionIDs;
};
