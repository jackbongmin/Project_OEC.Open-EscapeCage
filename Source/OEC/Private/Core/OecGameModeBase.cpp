// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OecGameModeBase.h"
#include "Core/OecPlayerController.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"

AOecGameModeBase::AOecGameModeBase()
{
	PlayerControllerClass = AOecPlayerController::StaticClass();
}
