// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Item/Weapon/OecWeaponBase.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/OecDataStruct.h"

// Sets default values
AOecWeaponBase::AOecWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AOecWeaponBase::InitWeaponData(const FItemStaticData& InWeaponData)
{

	BaseDamage = InWeaponData.Damage;
}

void AOecWeaponBase::Equip(AOecPlayerCharacter* InOwnerCharacter, FName InSocketName)
{
	if (!InOwnerCharacter)
	{
		return;
	}
	OwnerCharacter = InOwnerCharacter;
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
}

void AOecWeaponBase::Unequip()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	OwnerCharacter = nullptr;
}

void AOecWeaponBase::StartAttack()
{
}

void AOecWeaponBase::StopAttack()
{
}

FTransform AOecWeaponBase::GetLeftGripTransform() const
{
	if (WeaponMesh && WeaponMesh->DoesSocketExist(LeftGripSocketName))
	{
		// 1. 총에 있는 'Grip' 소켓의 진짜 월드 좌표
		FTransform gripWorld = WeaponMesh->GetSocketTransform(LeftGripSocketName, RTS_World);

		// 2. 내 캐릭터의 '오른손(hand_r)' 월드 좌표
		// (만약 애니메이션 뼈대 이름이 hand_r이 아니면 네 뼈대 이름으로 바꿔줘!)
		FTransform rightHandWorld = OwnerCharacter->GetMesh()->GetSocketTransform(TEXT("hand_r"), RTS_World);

		// 3. 오른손을 기준으로 한 'Grip'의 상대 위치(거리와 각도)를 계산해서 반환!
		return gripWorld.GetRelativeTransform(rightHandWorld);

		//return WeaponMesh->GetSocketTransform(LeftGripSocketName, RTS_World);
	}

	return FTransform();
}
