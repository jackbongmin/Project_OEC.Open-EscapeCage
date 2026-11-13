// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/Interact/OecDoorBase.h"
#include "Subsystems/OecEventSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"

AOecDoorBase::AOecDoorBase()
{
    DoorType = EDoorType::RandomDoor;
    RequiredStacks = 1;
    bShouldConsumeStack = true;
    bIsOpen = false;
}

void AOecDoorBase::BeginPlay()
{
    Super::BeginPlay();

    if (DoorType != EDoorType::MainDoor) return;

    if (UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>())
    {
        stageManager->OnPhaseChanged.AddUniqueDynamic(this, &AOecDoorBase::OnPhaseChanged);
    }

    // 이미 페이즈 2인 상태에서 스폰/스트리밍된 경우 바로 열어둠
    OnPhaseChanged();
}

void AOecDoorBase::OnPhaseChanged()
{
    if (DoorType != EDoorType::MainDoor || bIsOpen || !IsPhase2()) return;

    SetDoorState(true);
    SetInteractPromptVisible(false);
    UE_LOG(LogTemp, Log, TEXT("[%s] 페이즈 2 진입으로 메인 문 개방"), *GetName());
}

bool AOecDoorBase::IsPhase2() const
{
    const UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>();
    return stageManager && stageManager->GetCurrentPhase() == EOecGamePhase::Phase2_Corridor;
}

void AOecDoorBase::SetDoorState(bool bInOpen)
{
    bIsOpen = bInOpen;
    BP_OnDoorStateChanged(bIsOpen);
}

void AOecDoorBase::SetDoorLocked(bool bInLocked)
{
    bIsLocked = bInLocked;

    if (bIsLocked)
    {
        SetInteractPromptVisible(false);
    }
}

void AOecDoorBase::Interact_Implementation(AActor* InInteractor)
{
    Super::Interact_Implementation(InInteractor);

    if (bIsOpen || bIsLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 문이 열려있거나 잠겨있습니다"), *GetName());
        return;
    }

    // 메인 문은 열쇠 스택을 쓰지 않음. 페이즈 1 전구가 다 켜져야만 열림
    if (DoorType == EDoorType::MainDoor)
    {
        if (IsPhase2())
        {
            OnPhaseChanged();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] 메인 문은 전구 5개가 모두 켜져야 열립니다"), *GetName());
        }
        return;
    }

    UOecEventSubsystem* eventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>();
    if (!eventSubsystem) return;

    int32 currentStack = eventSubsystem->GetCurrentStacks();

    // 스택 유무 확인
    if (currentStack >= RequiredStacks)
    {
        // 소모 설정이 켜져 있을 때만 스택 차감 (페이즈 2 랜덤문 전용)
        if (bShouldConsumeStack)
        {
            eventSubsystem->ModifyStacks(-RequiredStacks);
        }

        SetDoorState(true);
		SetInteractPromptVisible(false);
        UE_LOG(LogTemp, Log, TEXT("[%s] 개방 성공"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 스택 부족 필요: %d, 현재: %d"), *GetName(), RequiredStacks, currentStack);
        // 나중에 여기에 화면 흔들림이나 "띠딕!" 하는 거절 사운드 재생하면 됨
    }

}

