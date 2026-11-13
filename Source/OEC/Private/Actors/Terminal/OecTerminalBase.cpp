// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Terminal/OecTerminalBase.h"
#include "Actors/Interactable/Interact/OecQuestionButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Ingame/OecTerminalWidget.h"
#include "Data/OecDataStruct.h"

#include "Subsystems/OecEventSubsystem.h"
#include "Choice/OecChoiceSubsystem.h"

// Sets default values
AOecTerminalBase::AOecTerminalBase()
{
	PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    ScreenWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ScreenWidget"));
    ScreenWidget->SetupAttachment(RootComponent);

    ScreenWidget->SetWidgetSpace(EWidgetSpace::World);
}

void AOecTerminalBase::BeginPlay()
{
	Super::BeginPlay();

    InitializeTerminal();

    if (RedButton)
    {
        RedButton->OnButtonClicked.AddDynamic(this, &AOecTerminalBase::InitializeTerminal);
    }

    if (BlueButton)
    {
        BlueButton->OnButtonClicked.AddDynamic(this, &AOecTerminalBase::InitializeTerminal);
    }
	
}

void AOecTerminalBase::InitializeTerminal()
{
    if (!QuestionDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("단말기에 질문 데이터 테이블이 연결되지 않았음"));
        return;
    }

    UOecEventSubsystem* eventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>();
    if (!eventSubsystem) return;

    // 타입별로 질문을 받을 준비
    TMap<EQuestionType, TArray<FQuestionStaticData*>> categorizedQuestions;

    // 데이터 테이블의 모든 행 데이터를 가져오기
    TArray<FQuestionStaticData*> allRows;
    QuestionDataTable->GetAllRows(TEXT("TerminalInit"), allRows);

    if (allRows.Num() == 0) return;

    TArray<FQuestionStaticData*> filteredRows;
    for (FQuestionStaticData* row : allRows)
    {
        // 널 포인터 체크 & 서브시스템에 아직 안 쓴 질문인지 ID로 확인!
        if (row && !eventSubsystem->IsQuestionUsed(row->QuestionID))
        {
            filteredRows.Add(row);
        }
    }

    // 만약 모든 질문을 다 써버렸다면?
    if (filteredRows.Num() == 0)
    {
        eventSubsystem->ResetUsedQuestions();
        filteredRows = allRows; // 다시 전체 리스트로 복구
    }

    // 이제 filteredRows 안에서 타입별 분류 및 확률 계산 진행!
    for (FQuestionStaticData* row : filteredRows)
    {
        categorizedQuestions.FindOrAdd(row->QuestionType).Add(row);
    }

    // 전체 확률 총합 구하기
    float totalWeight = 0.0f;
    for (const auto& pair : categorizedQuestions)
    {
        if (QuestionTypeWeights.Contains(pair.Key) && pair.Value.Num() > 0)
        {
            totalWeight += QuestionTypeWeights[pair.Key];
        }
    }

    if (totalWeight <= 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("단말기 확률 세팅이 안 되어 있거나 데이터가 없습니다!"));
        return;
    }

    // 주사위 굴리기
    float randomRoll = FMath::FRandRange(0.0f, totalWeight);
    EQuestionType selectedType = EQuestionType::Balance;
    float accumulatedWeight = 0.0f;

    // 어떤 타입에 당첨되었는지 검사
    for (const auto& pair : categorizedQuestions)
    {
        if (QuestionTypeWeights.Contains(pair.Key) && pair.Value.Num() > 0)
        {
            accumulatedWeight += QuestionTypeWeights[pair.Key];
            if (randomRoll <= accumulatedWeight)
            {
                selectedType = pair.Key;
                break;
            }
        }
    }

    // 당첨된 타입의 바구니 안에서 진짜 질문 1개 랜덤으로 뽑기
    TArray<FQuestionStaticData*>& availableQuestions = categorizedQuestions[selectedType];
    int32 randomIndex = FMath::RandRange(0, availableQuestions.Num() - 1);
    FQuestionStaticData* finalQuestion = availableQuestions[randomIndex];

    // 단말기 로그 출력 및 버튼에 데이터 덮어씌우기
    if (finalQuestion)
    {
        UE_LOG(LogTemp, Log, TEXT("===================================="));
        UE_LOG(LogTemp, Log, TEXT("뽑힌 타입: %d | 단말기 화면: %s"), (int32)selectedType, *finalQuestion->QuestionText);
        UE_LOG(LogTemp, Log, TEXT("===================================="));

        // 버튼 문구는 선택지 데이터에서 생성 (DisplayText가 비어 있으면 효과로 자동 생성)
        UOecChoiceSubsystem* choiceSubsystem = GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>();
        const FString redText = choiceSubsystem ? choiceSubsystem->BuildChoiceText(finalQuestion->RedChoice) : finalQuestion->RedChoice.DisplayText;
        const FString blueText = choiceSubsystem ? choiceSubsystem->BuildChoiceText(finalQuestion->BlueChoice) : finalQuestion->BlueChoice.DisplayText;

        if (UOecTerminalWidget* terminalUI = Cast<UOecTerminalWidget>(ScreenWidget->GetUserWidgetObject()))
        {
            terminalUI->UpdateTerminalUI(finalQuestion->QuestionText, redText, blueText);
            UE_LOG(LogTemp, Log, TEXT("UI 업데이트 성공"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UI 캐스트 실패 단말기 위젯 클래스가 잘못 지정되었습니다."));
        }

        if (RedButton)
        {
            RedButton->SetChoiceData(finalQuestion->QuestionID, finalQuestion->RedChoice);
        }

        if (BlueButton)
        {
            BlueButton->SetChoiceData(finalQuestion->QuestionID, finalQuestion->BlueChoice);
        }
    }
}


// Called every frame
void AOecTerminalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

