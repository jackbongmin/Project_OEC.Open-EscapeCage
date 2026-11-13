// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/Interact/OecQuestionButton.h"
#include "Choice/OecChoiceSubsystem.h"

void AOecQuestionButton::Interact_Implementation(AActor* InInteractor)
{
    Super::Interact_Implementation(InInteractor);

    PlayButtonPressFeedback();
    TriggerButtonEvent(InInteractor);

    if (OnButtonClicked.IsBound())
    {
        OnButtonClicked.Broadcast();
    }
}

void AOecQuestionButton::SetChoiceData(FName InQuestionID, const FOecChoiceData& InChoiceData)
{
    QuestionID = InQuestionID;
    ChoiceData = InChoiceData;

    UE_LOG(LogTemp, Log, TEXT("[%s] 선택지 갱신 완료 (결과 %d개)"), *GetName(), ChoiceData.Outcomes.Num());
}

void AOecQuestionButton::PlayButtonPressFeedback()
{
    UE_LOG(LogTemp, Log, TEXT("버튼 눌림"));
}

void AOecQuestionButton::TriggerButtonEvent(AActor* InInteractor)
{
    if (!bIsTerminalButton)
    {
        UE_LOG(LogTemp, Log, TEXT("일반 상호작용 버튼입니다. 선택지 로직을 무시합니다."));
        return;
    }

    if (UOecChoiceSubsystem* choiceSubsystem = GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>())
    {
        choiceSubsystem->SubmitChoice(QuestionID, ChoiceData, InInteractor);
    }
}
