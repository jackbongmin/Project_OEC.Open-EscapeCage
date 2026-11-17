// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable/OecInteractableBase.h"
#include "Data/OecChoiceTypes.h"
#include "OecQuestionButton.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedDelegate);

UCLASS()
class OEC_API AOecQuestionButton : public AOecInteractableBase
{
	GENERATED_BODY()

public:
    virtual void Interact_Implementation(AActor* InInteractor) override;

    // 단말기가 질문을 뽑을 때마다 이 버튼의 선택지를 갱신
    UFUNCTION(BlueprintCallable, Category = "OEC|Event")
    void SetChoiceData(FName InQuestionID, const FOecChoiceData& InChoiceData, EOecChoiceButton InButtonSide = EOecChoiceButton::None);

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OEC|Event Data")
    FName QuestionID;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OEC|Event Data")
    FOecChoiceData ChoiceData;

    // 단말기의 어느 쪽 버튼인지 (연쇄 질문의 선행 조건 기록용)
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OEC|Event Data")
    EOecChoiceButton ButtonSide = EOecChoiceButton::None;

    UPROPERTY(EditAnywhere, Category = "OEC|Event")
    bool bIsTerminalButton = true;

public:
    UPROPERTY(BlueprintAssignable, Category = "OEC|Event")
    FOnButtonClickedDelegate OnButtonClicked;

private:
    void PlayButtonPressFeedback();
    void TriggerButtonEvent(AActor* InInteractor);

};
