// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/Interact/OecDoorBase.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Settings/OecGameDataSettings.h"

AOecDoorBase::AOecDoorBase()
{
    DoorType = EDoorType::RandomDoor;
    RequiredKeyCount = 1;
    bIsOpen = false;
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

bool AOecDoorBase::CanInteract_Implementation(AActor* InInteractor) const
{
    // 열린 문은 E 대상에서 빠짐 (잠긴 문은 잠김 안내를 위해 대상으로 둠)
    return !bIsOpen && Super::CanInteract_Implementation(InInteractor);
}

void AOecDoorBase::Interact_Implementation(AActor* InInteractor)
{
    Super::Interact_Implementation(InInteractor);

    if (bIsOpen || bIsLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 문이 열려있거나 잠겨있습니다"), *GetName());
        return;
    }

    UOecInventorySubsystem* inventorySubsystem = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
    if (!inventorySubsystem) return;

    // 1. 열쇠 확인
    const FName keyItemCode = GetDefault<UOecGameDataSettings>()->KeyItemCode;
    const int32 currentKeyCount = inventorySubsystem->GetItemQuantity(keyItemCode);

    if (currentKeyCount < RequiredKeyCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] 열쇠 부족 (필요 %d, 보유 %d)"), *GetName(), RequiredKeyCount, currentKeyCount);
        BP_OnOpenDenied(currentKeyCount, RequiredKeyCount);
        return;
    }

    // 2. 열쇠 소모 후 개방
    if (RequiredKeyCount > 0)
    {
        inventorySubsystem->RemoveItem(keyItemCode, RequiredKeyCount);
    }

    SetDoorState(true);
    SetInteractPromptVisible(false);
    UE_LOG(LogTemp, Log, TEXT("[%s] 열쇠 %d개로 개방"), *GetName(), RequiredKeyCount);
}
