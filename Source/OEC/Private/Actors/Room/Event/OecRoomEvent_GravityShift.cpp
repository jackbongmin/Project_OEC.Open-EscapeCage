// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_GravityShift.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AOecRoomEvent_GravityShift::AOecRoomEvent_GravityShift()
{
}

void AOecRoomEvent_GravityShift::OnEventStarted()
{
	if (const ACharacter* character = Cast<ACharacter>(GetTargetPlayer()))
	{
		OriginalGravityScale = character->GetCharacterMovement()->GravityScale;
		character->GetCharacterMovement()->GravityScale = GravityScale;
	}
}

void AOecRoomEvent_GravityShift::OnEventFinished(bool bInSuccess)
{
	if (const ACharacter* character = Cast<ACharacter>(GetTargetPlayer()))
	{
		character->GetCharacterMovement()->GravityScale = OriginalGravityScale;
	}
}
