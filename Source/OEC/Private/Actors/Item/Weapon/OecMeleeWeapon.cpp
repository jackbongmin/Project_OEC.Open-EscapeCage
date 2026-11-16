// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Item/Weapon/OecMeleeWeapon.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Data/OecDataStruct.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"

AOecMeleeWeapon::AOecMeleeWeapon()
{
	// 한손 무기 기본값. 양손 무기(대검 등)는 블루프린트에서 켠다
	bUseLeftHandIK = false;

	StaticWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticWeaponMesh"));
	StaticWeaponMesh->SetupAttachment(RootComponent);
	// 판정은 카메라 스윕으로 하므로 메시 콜리전은 끔 (플레이어 캡슐·시선 판정 방해 방지)
	StaticWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticWeaponMesh->SetGenerateOverlapEvents(false);
	StaticWeaponMesh->SetCanEverAffectNavigation(false);
}

void AOecMeleeWeapon::InitWeaponData(const FItemStaticData& InWeaponData)
{
	Super::InitWeaponData(InWeaponData);

	if (InWeaponData.AttackRange > 0.0f)
	{
		AttackRange = InWeaponData.AttackRange;
	}

	AttackMontage = InWeaponData.FireMontage;
}

void AOecMeleeWeapon::StartAttack()
{
	// 근접은 클릭 한 번에 한 번 휘두름 (휘두르는 중에는 어빌리티가 재발동을 막음)
	TriggerAttackAbility();
}

void AOecMeleeWeapon::StopAttack()
{
}

void AOecMeleeWeapon::TriggerAttackAbility()
{
	if (!OwnerCharacter) return;

	if (UAbilitySystemComponent* asc = OwnerCharacter->GetAbilitySystemComponent())
	{
		asc->AbilityLocalInputPressed(static_cast<int32>(EOecAbilityInputID::Fire));
		asc->AbilityLocalInputReleased(static_cast<int32>(EOecAbilityInputID::Fire));
	}
}

void AOecMeleeWeapon::PlaySwingFX()
{
	if (SwingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SwingSound, GetActorLocation());
	}
}

void AOecMeleeWeapon::PlayHitFX(const FHitResult& InHitResult)
{
	const FVector impactPoint = InHitResult.ImpactPoint.IsNearlyZero() ? InHitResult.Location : InHitResult.ImpactPoint;

	if (HitFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitFX, impactPoint, InHitResult.ImpactNormal.Rotation());
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, impactPoint);
	}

	if (HitCameraShakeClass && OwnerCharacter)
	{
		if (APlayerController* pc = Cast<APlayerController>(OwnerCharacter->GetController()))
		{
			pc->ClientStartCameraShake(HitCameraShakeClass);
		}
	}
}
