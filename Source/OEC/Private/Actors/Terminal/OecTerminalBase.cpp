// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Terminal/OecTerminalBase.h"
#include "Actors/Interactable/Interact/OecQuestionButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Ingame/OecTerminalWidget.h"
#include "Data/OecDataStruct.h"

#include "Question/OecQuestionSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
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

    // 질문 변경권을 쓰면 새 질문을 뽑는다
    if (UOecQuestionSubsystem* questionSubsystem = GetGameInstance()->GetSubsystem<UOecQuestionSubsystem>())
    {
        questionSubsystem->OnQuestionRerollRequested.AddDynamic(this, &AOecTerminalBase::InitializeTerminal);
    }
}

void AOecTerminalBase::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
    if (UOecQuestionSubsystem* questionSubsystem = GetGameInstance()->GetSubsystem<UOecQuestionSubsystem>())
    {
        questionSubsystem->OnQuestionRerollRequested.RemoveDynamic(this, &AOecTerminalBase::InitializeTerminal);
    }

    Super::EndPlay(InEndPlayReason);
}

void AOecTerminalBase::InitializeTerminal()
{
    if (!QuestionDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("단말기에 질문 데이터 테이블이 연결되지 않았음"));
        return;
    }

    // 어떤 질문을 낼지는 질문 서브시스템이 정한다 (필터와 선택기 규칙)
    UOecQuestionSubsystem* questionSubsystem = GetGameInstance()->GetSubsystem<UOecQuestionSubsystem>();
    if (!questionSubsystem) return;

    const FQuestionStaticData* finalQuestion = questionSubsystem->PickQuestion(QuestionDataTable, GetCurrentTypeWeights());

    // 단말기 로그 출력 및 버튼에 데이터 덮어씌우기
    if (finalQuestion)
    {
        UE_LOG(LogTemp, Log, TEXT("===================================="));
        UE_LOG(LogTemp, Log, TEXT("뽑힌 타입: %d | 단말기 화면: %s"), (int32)finalQuestion->QuestionType, *finalQuestion->QuestionText);
        UE_LOG(LogTemp, Log, TEXT("===================================="));

        // 버튼 문구는 선택지 데이터에서 생성 (DisplayText가 비어 있으면 효과로 자동 생성)
        // (WARDEN의 메모가 있으면 확률 결과를 미리 굴려 문구에 보여 준다)
        UOecChoiceSubsystem* choiceSubsystem = GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>();
        if (choiceSubsystem)
        {
            choiceSubsystem->PresentQuestion(*finalQuestion);
        }

        const FString redText = choiceSubsystem ? choiceSubsystem->BuildPresentedChoiceText(finalQuestion->QuestionID, EOecChoiceButton::Red, finalQuestion->RedChoice) : finalQuestion->RedChoice.DisplayText;
        const FString blueText = choiceSubsystem ? choiceSubsystem->BuildPresentedChoiceText(finalQuestion->QuestionID, EOecChoiceButton::Blue, finalQuestion->BlueChoice) : finalQuestion->BlueChoice.DisplayText;

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
            RedButton->SetChoiceData(finalQuestion->QuestionID, finalQuestion->RedChoice, EOecChoiceButton::Red);
        }

        if (BlueButton)
        {
            BlueButton->SetChoiceData(finalQuestion->QuestionID, finalQuestion->BlueChoice, EOecChoiceButton::Blue);
        }
    }
}


const TMap<EQuestionType, float>& AOecTerminalBase::GetCurrentTypeWeights() const
{
    const UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>();
    const bool bIsPhase2 = stageManager && stageManager->GetCurrentPhase() == EOecGamePhase::Phase2_Corridor;

    return (bIsPhase2 && Phase2QuestionTypeWeights.Num() > 0) ? Phase2QuestionTypeWeights : QuestionTypeWeights;
}

// Called every frame
void AOecTerminalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

