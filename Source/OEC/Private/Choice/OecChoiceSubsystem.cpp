// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/OecChoiceSubsystem.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "Subsystems/OecEventSubsystem.h"
#include "UObject/UObjectHash.h"

void UOecChoiceSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	InCollection.InitializeDependency<UOecEventSubsystem>();

	RegisterExecutors();
}

void UOecChoiceSubsystem::Deinitialize()
{
	Executors.Empty();
	AnswerModifiers.Empty();
	ChoiceHistory.Empty();

	Super::Deinitialize();
}

void UOecChoiceSubsystem::RegisterExecutors()
{
	// 실행기 하위 클래스를 모두 찾아 태그로 등록 (새 효과 추가 시 이 함수는 수정하지 않음)
	TArray<UClass*> executorClasses;
	GetDerivedClasses(UOecChoiceEffectExecutor::StaticClass(), executorClasses, true);

	for (UClass* executorClass : executorClasses)
	{
		if (!executorClass || executorClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists)) continue;

		UOecChoiceEffectExecutor* executor = NewObject<UOecChoiceEffectExecutor>(this, executorClass);
		const FGameplayTag effectTag = executor->GetEffectTag();

		if (!effectTag.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] 효과 태그가 지정되지 않은 실행기입니다"), *executorClass->GetName());
			continue;
		}

		if (Executors.Contains(effectTag))
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] 같은 태그의 실행기가 이미 있습니다: %s"), *executorClass->GetName(), *effectTag.ToString());
			continue;
		}

		Executors.Add(effectTag, executor);
	}

	UE_LOG(LogTemp, Log, TEXT("선택지 효과 실행기 %d개 등록 완료"), Executors.Num());
}

const UOecChoiceEffectExecutor* UOecChoiceSubsystem::FindExecutor(const FGameplayTag& InEffectTag) const
{
	const TObjectPtr<UOecChoiceEffectExecutor>* found = Executors.Find(InEffectTag);
	return found ? found->Get() : nullptr;
}

FOecChoiceEffectContext UOecChoiceSubsystem::MakeContext(AActor* InInstigator)
{
	FOecChoiceEffectContext context;
	context.Instigator = InInstigator;
	context.ChoiceSubsystem = this;
	return context;
}

void UOecChoiceSubsystem::SubmitChoice(FName InQuestionID, const FOecChoiceData& InChoice, AActor* InInstigator)
{
	if (UOecEventSubsystem* eventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>())
	{
		eventSubsystem->MarkQuestionUsed(InQuestionID);
	}

	const FOecChoiceEffectContext context = MakeContext(InInstigator);

	// 1. 이전 답변에서 걸린 지속 효과 진행 (이번 선택으로 새로 걸린 효과는 다음 답변부터 적용)
	TickAnswerModifiers(context);

	// 2. 결과 뽑기 → 효과 실행
	const int32 outcomeIndex = RollOutcomeIndex(InChoice);
	const FOecChoiceOutcome* outcome = InChoice.Outcomes.IsValidIndex(outcomeIndex) ? &InChoice.Outcomes[outcomeIndex] : nullptr;

	if (outcome)
	{
		ExecuteOutcome(context, *outcome);
	}

	// 3. 기록
	FOecChoiceRecord record;
	record.QuestionID = InQuestionID;
	record.ChoiceText = BuildChoiceText(InChoice);
	record.OutcomeIndex = outcomeIndex;
	ChoiceHistory.Add(record);

	UE_LOG(LogTemp, Log, TEXT("질문 [%s] 선택: %s → 결과 %d (누적 선택 %d회)"), *InQuestionID.ToString(), *record.ChoiceText, outcomeIndex, ChoiceHistory.Num());

	OnChoiceResolved.Broadcast(record, outcome ? *outcome : FOecChoiceOutcome());
}

void UOecChoiceSubsystem::ExecuteEffect(const FOecChoiceEffect& InEffect, AActor* InInstigator)
{
	const UOecChoiceEffectExecutor* executor = FindExecutor(InEffect.EffectTag);
	if (!executor)
	{
		UE_LOG(LogTemp, Error, TEXT("등록되지 않은 효과 태그입니다: %s"), *InEffect.EffectTag.ToString());
		return;
	}

	executor->Execute(MakeContext(InInstigator), InEffect);
}

void UOecChoiceSubsystem::ExecuteOutcome(const FOecChoiceEffectContext& InContext, const FOecChoiceOutcome& InOutcome)
{
	for (const FOecChoiceEffect& effect : InOutcome.Effects)
	{
		if (const UOecChoiceEffectExecutor* executor = FindExecutor(effect.EffectTag))
		{
			executor->Execute(InContext, effect);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("등록되지 않은 효과 태그입니다: %s"), *effect.EffectTag.ToString());
		}
	}
}

int32 UOecChoiceSubsystem::RollOutcomeIndex(const FOecChoiceData& InChoice) const
{
	if (InChoice.Outcomes.Num() == 0) return INDEX_NONE;
	if (InChoice.Outcomes.Num() == 1) return 0;

	float totalWeight = 0.0f;
	for (const FOecChoiceOutcome& outcome : InChoice.Outcomes)
	{
		totalWeight += FMath::Max(outcome.Weight, 0.0f);
	}

	if (totalWeight <= 0.0f) return INDEX_NONE;

	const float randomRoll = FMath::FRandRange(0.0f, totalWeight);
	float accumulatedWeight = 0.0f;

	for (int32 i = 0; i < InChoice.Outcomes.Num(); ++i)
	{
		accumulatedWeight += FMath::Max(InChoice.Outcomes[i].Weight, 0.0f);
		if (randomRoll <= accumulatedWeight)
		{
			return i;
		}
	}

	return InChoice.Outcomes.Num() - 1;
}

void UOecChoiceSubsystem::AddAnswerModifier(const FGameplayTag& InEffectTag, float InValue, int32 InRemainingAnswers, bool bInAccumulate)
{
	FOecAnswerModifier* modifier = AnswerModifiers.FindByPredicate([&InEffectTag](const FOecAnswerModifier& InModifier)
	{
		return InModifier.EffectTag == InEffectTag;
	});

	if (modifier)
	{
		modifier->Value = bInAccumulate ? modifier->Value + InValue : InValue;
		modifier->RemainingAnswers = InRemainingAnswers;
	}
	else
	{
		FOecAnswerModifier newModifier;
		newModifier.EffectTag = InEffectTag;
		newModifier.Value = InValue;
		newModifier.RemainingAnswers = InRemainingAnswers;
		modifier = &AnswerModifiers.Add_GetRef(newModifier);
	}

	UE_LOG(LogTemp, Log, TEXT("지속 효과 적용: %s (값 %g, 남은 답변 %d)"), *InEffectTag.ToString(), modifier->Value, modifier->RemainingAnswers);

	OnAnswerModifierChanged.Broadcast(*modifier, true);
}

void UOecChoiceSubsystem::TickAnswerModifiers(const FOecChoiceEffectContext& InContext)
{
	// 실행기가 틱 중에 다른 효과를 실행해도 안전하도록 복사본으로 순회
	const TArray<FOecAnswerModifier> modifiersSnapshot = AnswerModifiers;

	for (const FOecAnswerModifier& modifier : modifiersSnapshot)
	{
		if (const UOecChoiceEffectExecutor* executor = FindExecutor(modifier.EffectTag))
		{
			executor->OnAnswerTick(InContext, modifier);
		}
	}

	// 기간이 있는 효과는 1 감소, 끝나면 제거
	for (int32 i = AnswerModifiers.Num() - 1; i >= 0; --i)
	{
		FOecAnswerModifier& modifier = AnswerModifiers[i];
		if (modifier.IsPermanent()) continue;

		--modifier.RemainingAnswers;
		if (modifier.RemainingAnswers > 0) continue;

		const FOecAnswerModifier expiredModifier = modifier;
		AnswerModifiers.RemoveAt(i);

		if (const UOecChoiceEffectExecutor* executor = FindExecutor(expiredModifier.EffectTag))
		{
			executor->OnModifierExpired(InContext, expiredModifier);
		}

		UE_LOG(LogTemp, Log, TEXT("지속 효과 종료: %s"), *expiredModifier.EffectTag.ToString());
		OnAnswerModifierChanged.Broadcast(expiredModifier, false);
	}
}

FString UOecChoiceSubsystem::BuildChoiceText(const FOecChoiceData& InChoice) const
{
	if (!InChoice.DisplayText.IsEmpty()) return InChoice.DisplayText;
	if (InChoice.Outcomes.Num() == 0) return TEXT("");
	if (InChoice.Outcomes.Num() == 1) return BuildOutcomeText(InChoice.Outcomes[0]);

	float totalWeight = 0.0f;
	for (const FOecChoiceOutcome& outcome : InChoice.Outcomes)
	{
		totalWeight += FMath::Max(outcome.Weight, 0.0f);
	}

	if (totalWeight <= 0.0f) return TEXT("");

	// 확률 결과: "아무 일 없음" 결과는 생략하고 "N% 확률로 ..." 형태로 표시
	TArray<FString> parts;
	for (const FOecChoiceOutcome& outcome : InChoice.Outcomes)
	{
		if (IsEmptyOutcome(outcome)) continue;

		const int32 percent = FMath::RoundToInt(FMath::Max(outcome.Weight, 0.0f) / totalWeight * 100.0f);
		parts.Add(FString::Printf(TEXT("%d%% 확률로 %s"), percent, *BuildOutcomeText(outcome)));
	}

	return parts.Num() > 0 ? FString::Join(parts, TEXT(" / ")) : TEXT("아무 일도 일어나지 않음");
}

FString UOecChoiceSubsystem::BuildOutcomeText(const FOecChoiceOutcome& InOutcome) const
{
	TArray<FString> descriptions;
	for (const FOecChoiceEffect& effect : InOutcome.Effects)
	{
		if (const UOecChoiceEffectExecutor* executor = FindExecutor(effect.EffectTag))
		{
			descriptions.Add(executor->BuildDescription(effect));
		}
	}

	return descriptions.Num() > 0 ? FString::Join(descriptions, TEXT(", ")) : TEXT("아무 일도 일어나지 않음");
}

bool UOecChoiceSubsystem::IsEmptyOutcome(const FOecChoiceOutcome& InOutcome) const
{
	for (const FOecChoiceEffect& effect : InOutcome.Effects)
	{
		const UOecChoiceEffectExecutor* executor = FindExecutor(effect.EffectTag);
		if (executor && !executor->IsEmptyEffect()) return false;
	}

	return true;
}
