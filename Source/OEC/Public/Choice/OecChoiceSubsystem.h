// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/OecChoiceTypes.h"
#include "OecChoiceSubsystem.generated.h"

class UOecChoiceEffectExecutor;
struct FQuestionStaticData;

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
	void SubmitChoice(FName InQuestionID, const FOecChoiceData& InChoice, AActor* InInstigator, EOecChoiceButton InButton = EOecChoiceButton::None);

	// 효과 하나를 바로 실행 (치트, 지속 효과에서 사용)
	UFUNCTION(BlueprintCallable, Category = "OEC|Choice")
	void ExecuteEffect(const FOecChoiceEffect& InEffect, AActor* InInstigator);

	// 버튼 문구 (DisplayText가 비어 있으면 효과 데이터로 자동 생성)
	UFUNCTION(BlueprintPure, Category = "OEC|Choice")
	FString BuildChoiceText(const FOecChoiceData& InChoice) const;

	// 답변 단위 지속 효과 추가 (같은 태그 + Param이 있으면 누적하거나 갱신)
	void AddAnswerModifier(const FGameplayTag& InEffectTag, float InValue, int32 InRemainingAnswers, bool bInAccumulate, FName InParam = NAME_None);

	// 같은 태그 + Param의 지속 효과가 걸려 있는지
	bool HasAnswerModifier(const FGameplayTag& InEffectTag, FName InParam = NAME_None) const;

	// 충전식 지속 효과(Value = 남은 횟수)를 1 소모. 없으면 false (예: 소음 차단기, 보험 증서)
	bool ConsumeAnswerModifier(const FGameplayTag& InEffectTag, FName InParam = NAME_None);

	// 효과를 지금 실행할 수 있는지 (실행기의 CanExecute)
	bool CanExecuteEffect(const FOecChoiceEffect& InEffect, AActor* InInstigator);

	// 결과에 해로운 효과가 하나라도 있는지
	bool IsOutcomeHarmful(const FOecChoiceOutcome& InOutcome) const;

	// 단말기가 질문을 화면에 띄울 때 호출 (WARDEN의 메모가 충전되어 있으면 확률 결과를 미리 굴린다)
	void PresentQuestion(const FQuestionStaticData& InQuestion);

	// 화면에 띄운 질문의 버튼 문구 (미리 굴린 결과가 있으면 그 결과를 보여 줌)
	FString BuildPresentedChoiceText(FName InQuestionID, EOecChoiceButton InButton, const FOecChoiceData& InChoice) const;

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
	UFUNCTION()
	void OnRunStarted(int32 InRunCount);

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

	// 미리 굴린 결과 (질문 ID → X = 빨강 결과 번호, Y = 파랑 결과 번호)
	TMap<FName, FIntPoint> PreRolledOutcomes;
};
