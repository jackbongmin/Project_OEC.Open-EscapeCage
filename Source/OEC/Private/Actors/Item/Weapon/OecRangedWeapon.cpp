// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Item/Weapon/OecRangedWeapon.h"
#include "Data/OecDataStruct.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "GameplayEffect.h"

#include "GAS/Attributeset/OecPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"


AOecRangedWeapon::AOecRangedWeapon()
{
}

void AOecRangedWeapon::InitWeaponData(const FItemStaticData& InWeaponData)
{
	Super::InitWeaponData(InWeaponData);


	MaxAmmoInClip = InWeaponData.ClipSize;
	UseAmmoItemCode = InWeaponData.UseAmmoItemCode;
	ReloadMontage = InWeaponData.ReloadMontage;

	//CurrentAmmo = MaxAmmoInClip;
	CurrentAmmo = 0;
}

void AOecRangedWeapon::StartAttack()
{

	// 2. 첫 발은 즉시 발사
	TriggerFireAbility();

	// 3. 연사 설정 (FireRate 간격으로 Fire 함수를 반복 호출)
	if (FireRate > 0.0f)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AOecRangedWeapon::TriggerFireAbility, FireRate, true);
	}
}

void AOecRangedWeapon::StopAttack()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}


void AOecRangedWeapon::Zoom(bool bInIsZooming)
{
	if (!OwnerCharacter) return;

	if (APlayerController* pc = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		OwnerCharacter->SetAiming(bInIsZooming);

		// TODO: 조준하면 크로스헤어가 좁아지거나, 마우스 감도가 느려지는 기능도 나중에 여기에 추가하면 완벽해!
	}
}

void AOecRangedWeapon::PlayFireFX()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	if (MuzzleFlashFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashFX, WeaponMesh, TEXT("Muzzle"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}

	if (OwnerCharacter)
	{
		if (APlayerController* pc = Cast<APlayerController>(OwnerCharacter->GetController()))
		{
			if (FireCameraShakeClass)
			{
				pc->ClientStartCameraShake(FireCameraShakeClass);
			}
			float pitchRecoil = FMath::RandRange(MinPitchRecoil, MaxPitchRecoil);
			float yawRecoil = FMath::RandRange(-YawRecoilRange, YawRecoilRange);
			pc->AddPitchInput(pitchRecoil);
			pc->AddYawInput(yawRecoil);
		}
	}
}

void AOecRangedWeapon::PlayHitAndTracerFX(const FHitResult& InHitResult, const FVector& InTraceEnd, bool InbHit)
{
	FVector targetPoint = InbHit ? InHitResult.ImpactPoint : InTraceEnd;

	// 1. 벽/적에 맞았을 때 스파크(Impact) 튀기기
	if (InbHit && ImpactFX)
	{
		FRotator impactRotation = InHitResult.ImpactNormal.Rotation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, InHitResult.ImpactPoint, impactRotation);
	}

	// 2. 총구에서 날아가는 빛줄기(Tracer) 그리기
	if (TracerFX && WeaponMesh)
	{
		FVector muzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		float distanceFromMuzzle = FVector::Distance(muzzleLocation, targetPoint);

		if (distanceFromMuzzle > 200.0f)
		{
			FVector traceDir = (targetPoint - muzzleLocation).GetSafeNormal();
			float bulletSpeed = 20000.0f;
			float bulletLifetime = distanceFromMuzzle / bulletSpeed;

			UNiagaraComponent* tracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerFX, muzzleLocation);
			if (tracerComp)
			{
				tracerComp->SetVectorParameter(TEXT("Velocity"), traceDir * bulletSpeed);
				tracerComp->SetFloatParameter(TEXT("Lifetime"), bulletLifetime);
			}
		}
	}
}

void AOecRangedWeapon::TriggerFireAbility()
{
	if (OwnerCharacter)
	{
		if (UAbilitySystemComponent* asc = OwnerCharacter->GetAbilitySystemComponent())
		{
			asc->AbilityLocalInputPressed(static_cast<int32>(EOecAbilityInputID::Fire));
			asc->AbilityLocalInputReleased(static_cast<int32>(EOecAbilityInputID::Fire));
		}
	}
}

