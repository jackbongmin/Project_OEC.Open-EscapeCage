// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGA_WeaponFire.h"
#include "GAS/OecGameplayTags.h"
#include "Actors/Item/Weapon/OecRangedWeapon.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"


UOecGA_WeaponFire::UOecGA_WeaponFire()
{
	AbilityInputID = EOecAbilityInputID::Fire;
}

bool UOecGA_WeaponFire::CheckCost(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, OUT FGameplayTagContainer* OutOptionalRelevantTags) const
{
	if (!Super::CheckCost(InHandle, InActorInfo, OutOptionalRelevantTags)) return false;

	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	if (weapon && weapon->GetCurrentAmmo() > 0)
	{
		return true;
	}

	return false;
}

void UOecGA_WeaponFire::ApplyCost(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) const
{
	Super::ApplyCost(InHandle, InActorInfo, InActivationInfo);

	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	if (weapon)
	{
		weapon->ConsumeAmmo(1);
	}
}

void UOecGA_WeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

	if (!CommitAbility(InHandle, InActorInfo, InActivationInfo))
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	// 1. 발사 승인이 났으니 무기와 플레이어 정보 가져오기
	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	AOecPlayerCharacter* player = Cast<AOecPlayerCharacter>(InActorInfo->AvatarActor.Get());

	if (!weapon || !player || !DamageEffectClass)
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	// 2. 시각적 연출 실행 (총구 불꽃, 소리, 카메라 반동)
	weapon->PlayFireFX();

	// 3. 레이저 발사 (눈 기준)
	APlayerController* pc = Cast<APlayerController>(player->GetController());
	if (pc)
	{
		FVector cameraLocation;
		FRotator cameraRotation;
		pc->GetPlayerViewPoint(cameraLocation, cameraRotation);

		FVector traceStart = cameraLocation;
		FVector shootDirection = FMath::VRandCone(cameraRotation.Vector(), FMath::DegreesToRadians(weapon->SpreadAngle));
		FVector traceEnd = traceStart + (shootDirection * weapon->GetFireDistance());

		FHitResult hitResult;
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(player);
		queryParams.AddIgnoredActor(weapon);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, queryParams);

		weapon->PlayHitAndTracerFX(hitResult, traceEnd, bHit);

		if (bHit)
		{
			AActor* hitActor = hitResult.GetActor();
			UE_LOG(LogTemp, Warning, TEXT("🎯 1. 레이저 적중! 맞은 액터: %s"), hitActor ? *hitActor->GetName() : TEXT("None"));
			
			// 타겟 확인 및 데미지 GE 적용
			if (IAbilitySystemInterface* targetInterface = Cast<IAbilitySystemInterface>(hitResult.GetActor()))
			{
				UAbilitySystemComponent* targetASC = targetInterface->GetAbilitySystemComponent();
				UAbilitySystemComponent* sourceASC = InActorInfo->AbilitySystemComponent.Get();

				FGameplayTag monsterTag = OecGameplayTags::Character_Monster;

				UE_LOG(LogTemp, Warning, TEXT("🎯 2. 타겟 ASC 확인 완료! GE 주문서 굽기 시작..."));

				if (targetASC && sourceASC)
				{
					FGameplayEffectContextHandle contextHandle = sourceASC->MakeEffectContext();
					FGameplayEffectSpecHandle specHandle = sourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, contextHandle);

					if (specHandle.IsValid())
					{
						specHandle.Data.Get()->SetSetByCallerMagnitude(OecGameplayTags::Data_Damage, -weapon->GetBaseDamage());
						sourceASC->ApplyGameplayEffectSpecToTarget(*specHandle.Data.Get(), targetASC);

						UE_LOG(LogTemp, Warning, TEXT("🎯 몬스터 타격 성공! 데미지 전송됨."));
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ 레이저에 맞은 액터가 IAbilitySystemInterface 명찰이 없음!"));
		}
	}

	// 4. 발사 한 번 끝났으니 어빌리티 종료
	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}
