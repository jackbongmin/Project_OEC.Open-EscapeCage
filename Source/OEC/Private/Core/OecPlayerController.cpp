// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OecPlayerController.h"
#include "Core/OecCheatManager.h"

AOecPlayerController::AOecPlayerController()
{
	CheatClass = UOecCheatManager::StaticClass();
}

void AOecPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetUIInputMode(false);

#if !UE_BUILD_SHIPPING
	// 개발 빌드에서는 항상 치트 커맨드 사용 가능
	if (IsLocalController())
	{
		AddCheats(true);
	}
#endif
}
void AOecPlayerController::SetUIInputMode(bool InIsUIOnly)
{
	if (InIsUIOnly)
	{
		FInputModeGameAndUI inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(inputMode);
		bShowMouseCursor = true;

		SetIgnoreLookInput(true);
		SetIgnoreMoveInput(true);
	}
	else
	{
		FInputModeGameOnly inputMode;
		SetInputMode(inputMode);
		bShowMouseCursor = false;

		ResetIgnoreLookInput();
		ResetIgnoreMoveInput();
	}
}

