// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/Monster/OecMonsterBase.h"
#include "GAS/OecGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Subsystems/OecGameDataSubsystem.h"
#include "Data/OecDataStruct.h"

#include "GameplayAbilitySpec.h"
#include "GAS/Ability/OecGameplayAbility.h"

#include "Subsystems/OecPoolingSubsystem.h"
#include "AIController.h"
#include "BrainComponent.h"

AOecMonsterBase::AOecMonsterBase()
{
	AttributeSet = CreateDefaultSubobject<UOecAttributeSetBase>(TEXT("AttributeSet"));

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

void AOecMonsterBase::OnPoolActivate_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	bIsDead = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (AAIController* aiController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* brain = aiController->GetBrainComponent())
		{
			brain->RestartLogic();
		}
	}

	if (AbilitySystemComponent)
	{
		float maxHealth = AbilitySystemComponent->GetNumericAttribute(UOecAttributeSetBase::GetMaxHealthAttribute());
		AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetHealthAttribute(), maxHealth);
	}

	UE_LOG(LogTemp, Log, TEXT("✨ 몬스터 부활! (풀링 활성화)"));
}

void AOecMonsterBase::OnPoolDeactivate_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	StopAnimMontage();
}

void AOecMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetHealthAttribute(), 100.0f);
		AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetMaxHealthAttribute(), 100.0f);
	
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOecAttributeSetBase::GetHealthAttribute()).AddUObject(this, &AOecMonsterBase::OnHealthChanged);

		AbilitySystemComponent->AddLooseGameplayTag(OecGameplayTags::Character_Monster);
		
		UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
		if (dataSub && MonsterCode != NAME_None)
		{
			if (const FMonsterStaticData* monsterData = dataSub->GetMonsterData(MonsterCode))
			{
				AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetHealthAttribute(), monsterData->BaseHealth);
				AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetMaxHealthAttribute(), monsterData->BaseHealth);
				AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetAttackPowerAttribute(), monsterData->BaseDamage);

				if (HasAuthority())
				{
					for (TSubclassOf<UOecGameplayAbility> abilityClass : monsterData->DefaultAbilities)
					{
						if (abilityClass)
						{
							AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(abilityClass, 1, INDEX_NONE, this));
						}
					}
				}

				UE_LOG(LogTemp, Log, TEXT("✅ %s 몬스터 스탯 초기화 완료! (체력: %f, 공격력: %f)"), *monsterData->MonsterName, monsterData->BaseHealth, monsterData->BaseDamage);
			}
		}
		else
		{
			AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetHealthAttribute(), 100.0f);
			AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetMaxHealthAttribute(), 100.0f);
			AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetAttackPowerAttribute(), 10.0f);
			UE_LOG(LogTemp, Warning, TEXT("⚠️ 몬스터 코드가 설정되지 않았습니다! 기본값으로 세팅됨."));
		}
	}
}

void AOecMonsterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f && !bIsDead)
	{
		Die();
	}
}

void AOecMonsterBase::Die()
{
	bIsDead = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AAIController* aiController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* brain = aiController->GetBrainComponent())
		{
			brain->StopLogic("Monster Died");
		}
	}

	UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if (dataSub)
	{
		if (const FMonsterStaticData* monsterData = dataSub->GetMonsterData(MonsterCode))
		{
			if (monsterData->DeathMontage)
			{
				PlayAnimMontage(monsterData->DeathMontage);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("💀 몬스터 사망! 7초 뒤 시체가 사라집니다."));

	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AOecMonsterBase::ReturnToPoolTimer, 7.0f, false);
}

void AOecMonsterBase::ReturnToPoolTimer()
{
	if (UOecPoolingSubsystem* poolSub = GetWorld()->GetSubsystem<UOecPoolingSubsystem>())
	{
		poolSub->ReturnToPool(this);
		UE_LOG(LogTemp, Log, TEXT("♻️ 몬스터 시체 풀링 반납 완료."));
	}
}
