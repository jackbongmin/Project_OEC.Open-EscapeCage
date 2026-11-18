// Fill out your copyright notice in the Description page of Project Settings.


#include "Question/OecQuestionSubsystem.h"
#include "Question/Filters/OecQuestionFilter.h"
#include "Question/Selectors/OecQuestionSelector.h"

#include "Choice/OecChoiceSubsystem.h"
#include "Subsystems/OecEventSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "Subsystems/OecRunSubsystem.h"
#include "Settings/OecGameDataSettings.h"
#include "Engine/DataTable.h"
#include "UObject/UObjectHash.h"
#include "Algo/Sort.h"

void UOecQuestionSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	UOecChoiceSubsystem* choiceSubsystem = InCollection.InitializeDependency<UOecChoiceSubsystem>();
	UOecStageManagerSubsystem* stageManager = InCollection.InitializeDependency<UOecStageManagerSubsystem>();
	UOecEventSubsystem* eventSubsystem = InCollection.InitializeDependency<UOecEventSubsystem>();
	UOecRunSubsystem* runSubsystem = InCollection.InitializeDependency<UOecRunSubsystem>();

	RegisterRules();

	// 답변과 스택 변화를 구독해 출제에 필요한 기록을 쌓는다
	if (choiceSubsystem)
	{
		choiceSubsystem->OnChoiceResolved.AddDynamic(this, &UOecQuestionSubsystem::OnChoiceResolved);
	}

	if (stageManager)
	{
		LastPhase1Stack = stageManager->GetCurrentStack();
		stageManager->OnStackChanged.AddDynamic(this, &UOecQuestionSubsystem::OnPhase1StackChanged);
	}

	if (eventSubsystem)
	{
		LastKeyStacks = eventSubsystem->GetCurrentStacks();
		eventSubsystem->OnKeyStacksChanged.AddDynamic(this, &UOecQuestionSubsystem::OnKeyStacksChanged);
	}

	if (runSubsystem)
	{
		runSubsystem->OnRunStarted.AddDynamic(this, &UOecQuestionSubsystem::OnRunStarted);
	}
}

int32 UOecQuestionSubsystem::GetRunCount() const
{
	const UOecRunSubsystem* runSubsystem = GetGameInstance()->GetSubsystem<UOecRunSubsystem>();
	return runSubsystem ? runSubsystem->GetRunCount() : 1;
}

void UOecQuestionSubsystem::Deinitialize()
{
	Filters.Empty();
	Selectors.Empty();

	Super::Deinitialize();
}

void UOecQuestionSubsystem::RegisterRules()
{
	// 필터와 선택기의 하위 클래스를 모두 찾아 등록 (새 규칙 추가 시 이 함수는 수정하지 않음)
	TArray<UClass*> filterClasses;
	GetDerivedClasses(UOecQuestionFilter::StaticClass(), filterClasses, true);

	for (UClass* filterClass : filterClasses)
	{
		if (!filterClass || filterClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists)) continue;

		Filters.Add(NewObject<UOecQuestionFilter>(this, filterClass));
	}

	TArray<UClass*> selectorClasses;
	GetDerivedClasses(UOecQuestionSelector::StaticClass(), selectorClasses, true);

	for (UClass* selectorClass : selectorClasses)
	{
		if (!selectorClass || selectorClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists)) continue;

		Selectors.Add(NewObject<UOecQuestionSelector>(this, selectorClass));
	}

	Algo::Sort(Selectors, [](const TObjectPtr<UOecQuestionSelector>& InLeft, const TObjectPtr<UOecQuestionSelector>& InRight)
	{
		return InLeft->GetPriority() > InRight->GetPriority();
	});

	UE_LOG(LogTemp, Log, TEXT("질문 출제 규칙 등록 완료 (필터 %d개, 선택기 %d개)"), Filters.Num(), Selectors.Num());
}

FOecQuestionContext UOecQuestionSubsystem::MakeContext(const TMap<EQuestionType, float>& InTypeWeights) const
{
	FOecQuestionContext context;
	context.RunCount = GetRunCount();
	context.AnswersWithoutStack = AnswersWithoutStack;
	context.TypeWeights = &InTypeWeights;
	context.QuestionSubsystem = this;
	context.EventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>();
	context.Settings = GetDefault<UOecGameDataSettings>();

	if (const UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>())
	{
		context.PhaseNumber = stageManager->GetCurrentPhase() == EOecGamePhase::Phase1_MainRoom ? 1 : 2;
	}

	return context;
}

const FQuestionStaticData* UOecQuestionSubsystem::PickQuestion(const UDataTable* InQuestionTable, const TMap<EQuestionType, float>& InTypeWeights)
{
	if (!InQuestionTable) return nullptr;

	TArray<FQuestionStaticData*> allRows;
	InQuestionTable->GetAllRows(TEXT("PickQuestion"), allRows);
	if (allRows.Num() == 0) return nullptr;

	const FOecQuestionContext context = MakeContext(InTypeWeights);

	// 1차: 모든 필터 적용 → 2차: 완화 가능한 필터(연속 제한 등)를 풀고 다시 시도
	// 그래도 없으면 풀이 소진된 것이므로 사용 기록을 초기화하고 한 번 더 반복
	for (int32 attempt = 0; attempt < 2; ++attempt)
	{
		for (const bool bRelax : { false, true })
		{
			TArray<const FQuestionStaticData*> candidates;
			CollectCandidates(context, allRows, bRelax, candidates);

			if (const FQuestionStaticData* question = SelectFromCandidates(context, candidates))
			{
				PresentedQuestionTypes.Add(question->QuestionID, question->QuestionType);
				return question;
			}
		}

		if (attempt == 0)
		{
			if (UOecEventSubsystem* eventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>())
			{
				eventSubsystem->ResetUsedQuestions();
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("낼 수 있는 질문이 없습니다 (페이즈 %d, 회차 %d). 타입 가중치와 질문 조건을 확인하세요"), context.PhaseNumber, context.RunCount);
	return nullptr;
}

void UOecQuestionSubsystem::CollectCandidates(const FOecQuestionContext& InContext, const TArray<FQuestionStaticData*>& InRows, bool bInRelax, TArray<const FQuestionStaticData*>& OutCandidates) const
{
	for (const FQuestionStaticData* row : InRows)
	{
		if (!row) continue;

		bool bPassed = true;
		for (const UOecQuestionFilter* filter : Filters)
		{
			if (bInRelax && filter->IsRelaxable()) continue;

			if (!filter->PassesFilter(InContext, *row))
			{
				bPassed = false;
				break;
			}
		}

		if (bPassed)
		{
			OutCandidates.Add(row);
		}
	}
}

const FQuestionStaticData* UOecQuestionSubsystem::SelectFromCandidates(const FOecQuestionContext& InContext, const TArray<const FQuestionStaticData*>& InCandidates) const
{
	if (InCandidates.Num() == 0) return nullptr;

	for (const UOecQuestionSelector* selector : Selectors)
	{
		if (const FQuestionStaticData* question = selector->SelectQuestion(InContext, InCandidates))
		{
			UE_LOG(LogTemp, Log, TEXT("질문 [%s] 출제 (%s, 후보 %d개)"), *question->QuestionID.ToString(), *selector->GetClass()->GetName(), InCandidates.Num());
			return question;
		}
	}

	return nullptr;
}

void UOecQuestionSubsystem::OnRunStarted(int32 InRunCount)
{
	AnswersWithoutStack = 0;
	bStackGainedSinceLastAnswer = false;
	PresentedQuestionTypes.Empty();
	RunAnswers.Empty();
	AnswerTypeHistory.Empty();
	StoryTagCounts.Empty();

	// 스테이지·열쇠 초기화로 바뀐 스택을 다음 답변의 "스택 획득"으로 오인하지 않게 기준값도 맞춘다
	LastPhase1Stack = 0;
	if (const UOecEventSubsystem* eventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>())
	{
		LastKeyStacks = eventSubsystem->GetCurrentStacks();
	}

	UE_LOG(LogTemp, Log, TEXT("%d회차 시작: 회차 단위 질문 기록 초기화"), InRunCount);
}

EOecChoiceButton UOecQuestionSubsystem::GetRunAnswer(FName InQuestionID) const
{
	const EOecChoiceButton* button = RunAnswers.Find(InQuestionID);
	return button ? *button : EOecChoiceButton::None;
}

int32 UOecQuestionSubsystem::GetStoryTagCount(EOecStoryTag InStoryTag) const
{
	const int32* count = StoryTagCounts.Find(InStoryTag);
	return count ? *count : 0;
}

void UOecQuestionSubsystem::OnChoiceResolved(const FOecChoiceRecord& InRecord, const FOecChoiceOutcome& InOutcome)
{
	// 1. 회차 기록 (선행 조건, 연속 제한, 엔딩 판정에 사용)
	RunAnswers.Add(InRecord.QuestionID, InRecord.Button);

	if (const EQuestionType* questionType = PresentedQuestionTypes.Find(InRecord.QuestionID))
	{
		AnswerTypeHistory.Add(*questionType);
	}

	if (InRecord.StoryTag != EOecStoryTag::None)
	{
		StoryTagCounts.FindOrAdd(InRecord.StoryTag)++;
	}

	// 2. 스택 보정 카운트 (효과 실행이 끝난 뒤에 이 콜백이 불린다)
	AnswersWithoutStack = bStackGainedSinceLastAnswer ? 0 : AnswersWithoutStack + 1;
	bStackGainedSinceLastAnswer = false;
}

void UOecQuestionSubsystem::OnPhase1StackChanged(int32 InCurrentStack, int32 InMaxStack)
{
	if (InCurrentStack > LastPhase1Stack)
	{
		bStackGainedSinceLastAnswer = true;
	}

	LastPhase1Stack = InCurrentStack;
}

void UOecQuestionSubsystem::OnKeyStacksChanged(int32 InCurrentStacks)
{
	// 문을 열 때 소모되는 감소는 무시하고, 늘어난 경우만 스택 획득으로 본다
	if (InCurrentStacks > LastKeyStacks)
	{
		bStackGainedSinceLastAnswer = true;
	}

	LastKeyStacks = InCurrentStacks;
}
