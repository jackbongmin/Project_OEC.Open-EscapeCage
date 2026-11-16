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
#include "GameFramework/CharacterMovementComponent.h"

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
	GetWorldTimerManager().ClearTimer(DeathTimerHandle);

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	bIsDead = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 사망 몽타주가 남아 있으면 정리
	StopAnimMontage();

	if (UCharacterMovementComponent* movement = GetCharacterMovement())
	{
		movement->SetComponentTickEnabled(true);
		movement->SetMovementMode(MOVE_Walking);
	}

	if (AbilitySystemComponent)
	{
		float maxHealth = AbilitySystemComponent->GetNumericAttribute(UOecAttributeSetBase::GetMaxHealthAttribute());
		AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetHealthAttribute(), maxHealth);
	}

	// AI 컨트롤러가 구독해서 시야와 BT를 다시 켬
	SetMonsterActive(true);

	UE_LOG(LogTemp, Log, TEXT("✨ 몬스터 부활! (풀링 활성화)"));
}

void AOecMonsterBase::OnPoolDeactivate_Implementation()
{
	// 살아 있는 채로 반납될 수도 있음 (ReturnAllActiveActors)
	SetMonsterActive(false);

	GetWorldTimerManager().ClearTimer(DeathTimerHandle);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (UCharacterMovementComponent* movement = GetCharacterMovement())
	{
		movement->StopMovementImmediately();
		movement->DisableMovement();
		movement->SetComponentTickEnabled(false);
	}

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

	// 진행 중인 공격 등 어빌리티 중단
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

	if (UCharacterMovementComponent* movement = GetCharacterMovement())
	{
		movement->StopMovementImmediately();
		movement->DisableMovement();
	}

	// AI 컨트롤러가 구독해서 시야·BT·디버그 시야 표시를 끔
	SetMonsterActive(false);
	OnMonsterDied.Broadcast(this);

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

void AOecMonsterBase::SetMonsterActive(bool bInActive)
{
	if (bIsMonsterActive == bInActive) return;

	bIsMonsterActive = bInActive;
	OnMonsterActiveChanged.Broadcast(this, bInActive);
}
