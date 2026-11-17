// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/OecDataStruct.h"
#include "Data/OecQuestionTypes.h"
#include "OecQuestionSubsystem.generated.h"

class UDataTable;
class UOecQuestionFilter;
class UOecQuestionSelector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunStartedDelegate, int32, InRunCount);

/**
 * 질문 출제 서브시스템
 *
 * "다음에 어떤 질문을 낼지"만 담당한다. (선택 결과 처리는 UOecChoiceSubsystem)
 * - 필터(UOecQuestionFilter): 지금 낼 수 없는 질문을 걸러낸다. 예) 꺼 둔 질문, 페이즈 제한, 선행 조건
 * - 선택기(UOecQuestionSelector): 남은 후보에서 하나를 고른다. 우선순위가 높은 것부터 시도한다.
 *   예) 고정 순서(튜토리얼) → 스택 보정 → 타입 가중치 무작위
 * 필터와 선택기는 시작할 때 하위 클래스를 찾아 자동 등록한다.
 *   → 새 출제 규칙은 클래스 하나만 추가하면 되고, 이 서브시스템과 단말기는 수정하지 않는다.
 */
UCLASS()
class OEC_API UOecQuestionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;
	virtual void Deinitialize() override;

	// 다음 질문 뽑기 (낼 수 있는 질문이 없으면 nullptr)
	const FQuestionStaticData* PickQuestion(const UDataTable* InQuestionTable, const TMap<EQuestionType, float>& InTypeWeights);

	// 새 회차 시작 (회차 단위 기록 초기화, 회차 수 +1)
	UFUNCTION(BlueprintCallable, Category = "OEC|Question")
	void StartNewRun();

	// 이번 회차에 이 질문에 답한 버튼 (답하지 않았으면 None)
	UFUNCTION(BlueprintPure, Category = "OEC|Question")
	EOecChoiceButton GetRunAnswer(FName InQuestionID) const;

	UFUNCTION(BlueprintPure, Category = "OEC|Question")
	bool HasAnsweredThisRun(FName InQuestionID) const { return RunAnswers.Contains(InQuestionID); }

	UFUNCTION(BlueprintPure, Category = "OEC|Question")
	int32 GetRunCount() const { return RunCount; }

	UFUNCTION(BlueprintPure, Category = "OEC|Question")
	int32 GetAnswersWithoutStack() const { return AnswersWithoutStack; }

	// 이번 회차에 답한 질문의 타입 (답한 순서대로)
	const TArray<EQuestionType>& GetAnswerTypeHistory() const { return AnswerTypeHistory; }

	// 이번 회차 스토리 성향별 선택 횟수 (엔딩 판정용)
	UFUNCTION(BlueprintPure, Category = "OEC|Question")
	int32 GetStoryTagCount(EOecStoryTag InStoryTag) const;

public:
	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnRunStartedDelegate OnRunStarted;

protected:
	UFUNCTION()
	void OnChoiceResolved(const FOecChoiceRecord& InRecord, const FOecChoiceOutcome& InOutcome);

	UFUNCTION()
	void OnPhase1StackChanged(int32 InCurrentStack, int32 InMaxStack);

	UFUNCTION()
	void OnKeyStacksChanged(int32 InCurrentStacks);

private:
	void RegisterRules();

	FOecQuestionContext MakeContext(const TMap<EQuestionType, float>& InTypeWeights) const;

	// 필터를 통과한 후보 모으기 (bInRelax = 완화 가능한 필터는 건너뜀)
	void CollectCandidates(const FOecQuestionContext& InContext, const TArray<FQuestionStaticData*>& InRows, bool bInRelax, TArray<const FQuestionStaticData*>& OutCandidates) const;

	const FQuestionStaticData* SelectFromCandidates(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const;

private:
	UPROPERTY()
	TArray<TObjectPtr<UOecQuestionFilter>> Filters;

	// 우선순위 내림차순
	UPROPERTY()
	TArray<TObjectPtr<UOecQuestionSelector>> Selectors;

	// 현재 회차 (1부터)
	int32 RunCount = 1;

	// 스택을 얻지 못한 연속 답변 수
	int32 AnswersWithoutStack = 0;

	// 직전 답변 이후 스택이 늘었는지
	bool bStackGainedSinceLastAnswer = false;

	int32 LastPhase1Stack = 0;

	int32 LastKeyStacks = 0;

	// 화면에 낸 질문의 타입 (답변이 들어왔을 때 타입을 알기 위함)
	TMap<FName, EQuestionType> PresentedQuestionTypes;

	// 이번 회차에 답한 질문 → 버튼
	TMap<FName, EOecChoiceButton> RunAnswers;

	TArray<EQuestionType> AnswerTypeHistory;

	TMap<EOecStoryTag, int32> StoryTagCounts;
};
