// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGA_MonsterAttack.h"
#include "GAS/OecGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/Characters/Monster/OecMonsterBase.h"
#include "Subsystems/OecGameDataSubsystem.h"

UOecGA_MonsterAttack::UOecGA_MonsterAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOecGA_MonsterAttack::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

	if (!CommitAbility(InHandle, InActorInfo, InActivationInfo))
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	AOecMonsterBase* monster = Cast<AOecMonsterBase>(GetAvatarActorFromActorInfo());
	UOecGameDataSubsystem* dataSub = GetWorld()->GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();

	if (monster && dataSub)
	{
		if (const FMonsterStaticData* monsterData = dataSub->GetMonsterData(monster->GetMonsterCode()))
		{
			if (monsterData->AttackMontage)
			{
				UAbilityTask_PlayMontageAndWait* montageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, monsterData->AttackMontage);
				montageTask->OnBlendOut.AddDynamic(this, &UOecGA_MonsterAttack::OnMontageCompleted);
				montageTask->OnCompleted.AddDynamic(this, &UOecGA_MonsterAttack::OnMontageCompleted);
				montageTask->OnInterrupted.AddDynamic(this, &UOecGA_MonsterAttack::OnMontageCompleted);
				montageTask->OnCancelled.AddDynamic(this, &UOecGA_MonsterAttack::OnMontageCompleted);
				montageTask->ReadyForActivation();

				FGameplayTag eventTag = OecGameplayTags::Event_Monster_Attack;
				UAbilityTask_WaitGameplayEvent* eventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, eventTag);
				eventTask->EventReceived.AddDynamic(this, &UOecGA_MonsterAttack::OnEventReceived);
				eventTask->ReadyForActivation();

				return;
			}
		}
	}

	EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
}

void UOecGA_MonsterAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOecGA_MonsterAttack::OnEventReceived(FGameplayEventData InPayload)
{
	AOecMonsterBase* monster = Cast<AOecMonsterBase>(GetAvatarActorFromActorInfo());
	UAbilitySystemComponent* asc = GetAbilitySystemComponentFromActorInfo();
	UOecGameDataSubsystem* dataSub = GetWorld()->GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();

	if (!monster || !asc || !dataSub) return;

	const FMonsterStaticData* monsterData = dataSub->GetMonsterData(monster->GetMonsterCode());
	if (!monsterData) return;

	float attackPower = asc->GetNumericAttribute(UOecAttributeSetBase::GetAttackPowerAttribute());

	FVector startLoc = monster->GetMesh()->GetSocketLocation(monsterData->AttackSocketName);

	FVector endLoc = startLoc + (monster->GetActorForwardVector() * monsterData->AttackRange);

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(monster);

	FHitResult hitResult;
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(monster, startLoc, endLoc, monsterData->AttackRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true);


	/*FVector startLoc = monster->GetActorLocation();
	FVector endLoc = startLoc + (monster->GetActorForwardVector() * monsterData->AttackRange);

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(monster);

	FHitResult hitResult;
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(monster, startLoc, endLoc, monsterData->AttackRadius, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true);*/

	if (bHit)
	{
		if (IAbilitySystemInterface* targetInterface = Cast<IAbilitySystemInterface>(hitResult.GetActor()))
		{
			UAbilitySystemComponent* targetAsc = targetInterface->GetAbilitySystemComponent();
			if (targetAsc)
			{
				FGameplayEffectContextHandle contextHandle = asc->MakeEffectContext();
				FGameplayEffectSpecHandle specHandle = asc->MakeOutgoingSpec(DamageEffectClass, 1.0f, contextHandle);

				if (specHandle.IsValid())
				{
					specHandle.Data.Get()->SetSetByCallerMagnitude(OecGameplayTags::Data_Damage, -attackPower);
					asc->ApplyGameplayEffectSpecToTarget(*specHandle.Data.Get(), targetAsc);

					UE_LOG(LogTemp, Warning, TEXT("💥 %s 펀치 적중! (사거리: %f, 피해량: %f)"), *monsterData->MonsterName, monsterData->AttackRange, attackPower);
				}
			}
		}
	}
}
