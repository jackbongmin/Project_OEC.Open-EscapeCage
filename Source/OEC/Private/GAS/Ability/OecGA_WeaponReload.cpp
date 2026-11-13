// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGA_WeaponReload.h"
#include "Actors/Item/Weapon/OecRangedWeapon.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" 

UOecGA_WeaponReload::UOecGA_WeaponReload()
{
	AbilityInputID = EOecAbilityInputID::Reload;
}

bool UOecGA_WeaponReload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	if (!weapon) return false;

	if (weapon->GetCurrentAmmo() >= weapon->GetMaxAmmoInClip()) return false;

	UOecInventorySubsystem* invenSub = GetWorld()->GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
	if (invenSub && invenSub->GetItemQuantity(weapon->GetUseAmmoItemCode()) <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ 인벤토리에 예비 총알이 없습니다!"));
		return false;
	}

	return true;
}

void UOecGA_WeaponReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	UOecInventorySubsystem* invenSub = GetWorld()->GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();

	if (!weapon || !invenSub)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	int32 neededAmmo = weapon->GetMaxAmmoInClip() - weapon->GetCurrentAmmo();
	int32 availableAmmo = invenSub->GetItemQuantity(weapon->GetUseAmmoItemCode());

	CachedReloadAmount = FMath::Min(neededAmmo, availableAmmo);

	UAnimMontage* reloadMontage = weapon->GetReloadMontage();
	if (reloadMontage)
	{
		UAbilityTask_PlayMontageAndWait* task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, reloadMontage);

		task->OnBlendOut.AddDynamic(this, &UOecGA_WeaponReload::OnMontageCompleted);
		task->OnCompleted.AddDynamic(this, &UOecGA_WeaponReload::OnMontageCompleted);

		task->OnInterrupted.AddDynamic(this, &UOecGA_WeaponReload::OnMontageInterrupted);
		task->OnCancelled.AddDynamic(this, &UOecGA_WeaponReload::OnMontageInterrupted);

		task->ReadyForActivation();
	}
	else
	{
		OnMontageCompleted();
	}
}

void UOecGA_WeaponReload::OnMontageCompleted()
{
	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	UOecInventorySubsystem* invenSub = GetWorld()->GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();

	if (weapon && invenSub && CachedReloadAmount > 0)
	{
		invenSub->RemoveItem(weapon->GetUseAmmoItemCode(), CachedReloadAmount);
		weapon->AddAmmo(CachedReloadAmount);
		UE_LOG(LogTemp, Warning, TEXT("✅ 장전 완료! 탄창: %d발"), weapon->GetCurrentAmmo());
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOecGA_WeaponReload::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("❌ 장전 취소됨! (총알 안 들어감)"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
