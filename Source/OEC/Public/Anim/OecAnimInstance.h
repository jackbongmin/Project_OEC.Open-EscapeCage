// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/OecEnumType.h"
#include "OecAnimInstance.generated.h"

/**
 * 
 */

class AOecPlayerCharacter;
class UCharacterMovementComponent;

UCLASS()
class OEC_API UOecAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float InDeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<AOecPlayerCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	EOecWeaponState CurrentWeaponState;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float AimPitch;

	// 왼손 Grip IK 세기 (0~1). Two Bone IK 노드의 Alpha에 연결. 무기를 바꾸면 부드럽게 전환
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	float LeftHandIKAlpha = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float LeftHandIKBlendSpeed = 10.0f;


};
