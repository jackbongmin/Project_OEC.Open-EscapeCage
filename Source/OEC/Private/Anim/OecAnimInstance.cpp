// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/OecAnimInstance.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Actors/Characters/OecCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"


void UOecAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AOecCharacterBase* baseChar = Cast<AOecCharacterBase>(TryGetPawnOwner()))
	{
		MovementComponent = baseChar->GetCharacterMovement();

		OwnerCharacter = Cast<AOecPlayerCharacter>(baseChar);
	}
}

void UOecAnimInstance::NativeUpdateAnimation(float InDeltaSeconds)
{
	Super::NativeUpdateAnimation(InDeltaSeconds);

	// ==========================================
	// 💡 1. 몬스터 & 플레이어 공통 로직 (이동)
	// ==========================================
	if (MovementComponent && TryGetPawnOwner())
	{
		Speed = TryGetPawnOwner()->GetVelocity().Size2D();
		bIsMoving = (Speed > 3.0f) && (MovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.0f);
	}

	// ==========================================
	// 💡 2. 플레이어 전용 로직 (무기, 조준)
	// ==========================================
	if (OwnerCharacter) 
	{
		CurrentWeaponState = OwnerCharacter->CurrentWeaponState;

		FRotator AimRot = OwnerCharacter->GetBaseAimRotation();
		FRotator ActorRot = OwnerCharacter->GetActorRotation();

		FRotator DeltaRot = AimRot - ActorRot;
		DeltaRot.Normalize();
		AimPitch = DeltaRot.Pitch;
	
	}
}
