// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/OecChoiceTypes.h"
#include "OecChoiceSubsystem.generated.h"

class UOecChoiceEffectExecutor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceResolvedDelegate, const FOecChoiceRecord&, InRecord, const FOecChoiceOutcome&, InOutcome);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnswerModifierChangedDelegate, const FOecAnswerModifier&, InModifier, bool, bInActive);

/**
 * 선택지 서브시스템
 *
 * 버튼 선택 → 결과 뽑기(가중치) → 효과 실행 → 기록 의 흐름을 담당한다.
 * - 효과 실행은 태그별 실행기(UOecChoiceEffectExecutor)에 위임한다.
 * - "앞으로 답변할 때마다" 같은 지속 효과는 AnswerModifier로 관리한다.
 * - 결과는 델리게이트로 알리고, UI·연출·환경 액터가 구독한다.
 */
UCLASS()
class OEC_API UOecChoiceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;
	virtual void Deinitialize() override;

	// 버튼 선택 처리 (질문 사용 처리 → 지속 효과 진행 → 결과 뽑기 → 효과 실행 → 기록)
	UFUNCTION(BlueprintCallable, Category = "OEC|Choice")
	void SubmitChoice(FName InQuestionID, const FOecChoiceData& InChoice, AActor* InInstigator);

	// 효과 하나를 바로 실행 (치트, 지속 효과에서 사용)
	UFUNCTION(BlueprintCallable, Category = "OEC|Choice")
	void ExecuteEffect(const FOecChoiceEffect& InEffect, AActor* InInstigator);

	// 버튼 문구 (DisplayText가 비어 있으면 효과 데이터로 자동 생성)
	UFUNCTION(BlueprintPure, Category = "OEC|Choice")
	FString BuildChoiceText(const FOecChoiceData& InChoice) const;

	// 답변 단위 지속 효과 추가 (같은 태그가 있으면 누적하거나 갱신)
	void AddAnswerModifier(const FGameplayTag& InEffectTag, float InValue, int32 InRemainingAnswers, bool bInAccumulate);

	UFUNCTION(BlueprintPure, Category = "OEC|Choice")
	const TArray<FOecChoiceRecord>& GetChoiceHistory() const { return ChoiceHistory; }

	UFUNCTION(BlueprintPure, Category = "OEC|Choice")
	const TArray<FOecAnswerModifier>& GetAnswerModifiers() const { return AnswerModifiers; }

public:
	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnChoiceResolvedDelegate OnChoiceResolved;

	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnAnswerModifierChangedDelegate OnAnswerModifierChanged;

private:
	void RegisterExecutors();

	const UOecChoiceEffectExecutor* FindExecutor(const FGameplayTag& InEffectTag) const;

	FOecChoiceEffectContext MakeContext(AActor* InInstigator);

	int32 RollOutcomeIndex(const FOecChoiceData& InChoice) const;

	void ExecuteOutcome(const FOecChoiceEffectContext& InContext, const FOecChoiceOutcome& InOutcome);

	void TickAnswerModifiers(const FOecChoiceEffectContext& InContext);

	FString BuildOutcomeText(const FOecChoiceOutcome& InOutcome) const;

	bool IsEmptyOutcome(const FOecChoiceOutcome& InOutcome) const;

private:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UOecChoiceEffectExecutor>> Executors;

	UPROPERTY()
	TArray<FOecAnswerModifier> AnswerModifiers;

	UPROPERTY()
	TArray<FOecChoiceRecord> ChoiceHistory;
};
