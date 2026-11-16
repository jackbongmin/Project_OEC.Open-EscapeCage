// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGA_MeleeAttack.h"
#include "GAS/OecGameplayTags.h"
#include "Actors/Item/Weapon/OecMeleeWeapon.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "DrawDebugHelpers.h"

UOecGA_MeleeAttack::UOecGA_MeleeAttack()
{
	AbilityInputID = EOecAbilityInputID::Fire;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

AOecMeleeWeapon* UOecGA_MeleeAttack::GetMeleeWeapon() const
{
	return Cast<AOecMeleeWeapon>(GetCurrentSourceObject());
}

void UOecGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

	AOecMeleeWeapon* weapon = GetMeleeWeapon();
	if (!weapon || !DamageEffectClass || !CommitAbility(InHandle, InActorInfo, InActivationInfo))
	{
		if (!DamageEffectClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[GA_MeleeAttack] DamageEffectClass가 비어 있습니다"));
		}
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	bHitPerformed = false;
	weapon->PlaySwingFX();

	// 1. 몽타주가 없으면 바로 판정하고 일정 시간 뒤 종료
	UAnimMontage* attackMontage = weapon->GetAttackMontage();
	if (!attackMontage)
	{
		PerformHit();

		UAbilityTask_WaitDelay* delayTask = UAbilityTask_WaitDelay::WaitDelay(this, weapon->GetAttackIntervalWithoutMontage());
		delayTask->OnFinish.AddDynamic(this, &UOecGA_MeleeAttack::OnAttackIntervalFinished);
		delayTask->ReadyForActivation();
		return;
	}

	// 2. 노티파이 이벤트 대기 (몽타주보다 먼저 걸어야 첫 프레임 노티파이도 받음)
	UAbilityTask_WaitGameplayEvent* eventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, OecGameplayTags::Event_Weapon_MeleeHit);
	eventTask->EventReceived.AddDynamic(this, &UOecGA_MeleeAttack::OnHitEventReceived);
	eventTask->ReadyForActivation();

	// 3. 몽타주 재생
	UAbilityTask_PlayMontageAndWait* montageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, attackMontage);
	montageTask->OnBlendOut.AddDynamic(this, &UOecGA_MeleeAttack::OnMontageFinished);
	montageTask->OnCompleted.AddDynamic(this, &UOecGA_MeleeAttack::OnMontageFinished);
	montageTask->OnInterrupted.AddDynamic(this, &UOecGA_MeleeAttack::OnMontageFinished);
	montageTask->OnCancelled.AddDynamic(this, &UOecGA_MeleeAttack::OnMontageFinished);
	montageTask->ReadyForActivation();
}

void UOecGA_MeleeAttack::OnHitEventReceived(FGameplayEventData InPayload)
{
	PerformHit();
}

void UOecGA_MeleeAttack::OnMontageFinished()
{
	// 블렌드 아웃·완료가 연달아 올 수 있음
	if (!IsActive()) return;

	if (!bHitPerformed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GA_MeleeAttack] 공격 몽타주에 타격 노티파이(Event.Weapon.MeleeHit)가 없어 판정하지 않았습니다"));
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOecGA_MeleeAttack::OnAttackIntervalFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOecGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled)
{
	bHitPerformed = false;
	Super::EndAbility(InHandle, InActorInfo, InActivationInfo, bInReplicateEndAbility, bInWasCancelled);
}

void UOecGA_MeleeAttack::PerformHit()
{
	// 한 번 휘두를 때 판정은 한 번만 (노티파이가 중복돼도 안전)
	if (bHitPerformed) return;
	bHitPerformed = true;

	AOecMeleeWeapon* weapon = GetMeleeWeapon();
	AOecPlayerCharacter* player = Cast<AOecPlayerCharacter>(GetAvatarActorFromActorInfo());
	APlayerController* pc = player ? Cast<APlayerController>(player->GetController()) : nullptr;
	if (!weapon || !pc) return;

	// 1. 카메라 앞으로 구체 스윕 (Pawn 오브젝트만 = 몬스터)
	FVector viewLocation;
	FRotator viewRotation;
	pc->GetPlayerViewPoint(viewLocation, viewRotation);

	const FVector traceStart = viewLocation;
	const FVector traceEnd = viewLocation + viewRotation.Vector() * weapon->GetAttackRange();

	FCollisionQueryParams queryParams(SCENE_QUERY_STAT(OecMeleeSweep), false, player);
	queryParams.AddIgnoredActor(weapon);

	TArray<FHitResult> hitResults;
	GetWorld()->SweepMultiByObjectType(hitResults, traceStart, traceEnd, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(weapon->GetSweepRadius()), queryParams);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebugSweep)
	{
		DrawDebugCapsule(GetWorld(), (traceStart + traceEnd) * 0.5f, weapon->GetAttackRange() * 0.5f, weapon->GetSweepRadius(),
			FRotationMatrix::MakeFromZ(traceEnd - traceStart).ToQuat(), hitResults.Num() > 0 ? FColor::Red : FColor::Green, false, 1.0f);
	}
#endif

	// 2. 맞은 대상에 피해 (같은 액터는 한 번만, 벽 너머는 제외)
	TSet<AActor*> damagedActors;
	for (const FHitResult& hitResult : hitResults)
	{
		if (damagedActors.Num() >= weapon->GetMaxHitCount()) break;

		AActor* hitActor = hitResult.GetActor();
		if (!hitActor || damagedActors.Contains(hitActor)) continue;

		FHitResult wallHit;
		FCollisionQueryParams wallParams(SCENE_QUERY_STAT(OecMeleeWall), false, player);
		wallParams.AddIgnoredActor(weapon);
		wallParams.AddIgnoredActor(hitActor);
		if (GetWorld()->LineTraceSingleByChannel(wallHit, traceStart, hitResult.ImpactPoint, ECC_Visibility, wallParams))
		{
			continue;
		}

		damagedActors.Add(hitActor);
		ApplyDamageTo(hitActor, hitResult);
		weapon->PlayHitFX(hitResult);
	}

	UE_LOG(LogTemp, Log, TEXT("[GA_MeleeAttack] 휘두르기: %d명 적중 (피해 %.1f)"), damagedActors.Num(), weapon->GetBaseDamage());
}

void UOecGA_MeleeAttack::ApplyDamageTo(AActor* InTarget, const FHitResult& InHitResult)
{
	IAbilitySystemInterface* targetInterface = Cast<IAbilitySystemInterface>(InTarget);
	UAbilitySystemComponent* targetAsc = targetInterface ? targetInterface->GetAbilitySystemComponent() : nullptr;
	UAbilitySystemComponent* sourceAsc = GetAbilitySystemComponentFromActorInfo();
	AOecMeleeWeapon* weapon = GetMeleeWeapon();
	if (!targetAsc || !sourceAsc || !weapon) return;

	FGameplayEffectContextHandle contextHandle = sourceAsc->MakeEffectContext();
	contextHandle.AddSourceObject(weapon);
	contextHandle.AddHitResult(InHitResult);

	FGameplayEffectSpecHandle specHandle = sourceAsc->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), contextHandle);
	if (!specHandle.IsValid()) return;

	specHandle.Data.Get()->SetSetByCallerMagnitude(OecGameplayTags::Data_Damage, -weapon->GetBaseDamage());
	sourceAsc->ApplyGameplayEffectSpecToTarget(*specHandle.Data.Get(), targetAsc);
}
