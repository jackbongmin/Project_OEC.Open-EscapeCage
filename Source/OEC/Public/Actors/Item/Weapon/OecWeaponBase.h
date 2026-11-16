// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OecWeaponBase.generated.h"

class USkeletalMeshComponent;
class AOecPlayerCharacter;
struct FItemStaticData;


UCLASS()
class OEC_API AOecWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	AOecWeaponBase();

public:
	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
	virtual void InitWeaponData(const FItemStaticData& InWeaponData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintReadOnly, Category = "OEC|Weapon")
	AOecPlayerCharacter* OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Stats")
	float BaseDamage = 10.0f;

public:
	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
	virtual void Equip(AOecPlayerCharacter* InOwnerCharacter, FName InSocketName);

	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
	virtual void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
	virtual void StopAttack();

	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Sockets")
	FName LeftGripSocketName = TEXT("Grip");

	// 장착할 때 붙을 플레이어 메시 소켓 (총은 HandGrip_R, 근접 무기는 전용 소켓을 만들어 지정 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Sockets")
	FName EquipSocketName = TEXT("HandGrip_R");

public:
	UFUNCTION(BlueprintPure, Category = "OEC|Weapon")
	FName GetEquipSocketName() const { return EquipSocketName; }

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OEC|Weapon")
	FTransform GetLeftGripTransform() const;

	float GetBaseDamage() const { return BaseDamage; }

	// 왼손을 무기 Grip 소켓에 붙일지 (총 = 양손, 한손 검 = 끔). 애님 인스턴스가 IK 알파로 사용
	UFUNCTION(BlueprintPure, Category = "OEC|Weapon")
	bool UsesLeftHandIK() const { return bUseLeftHandIK; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Sockets")
	bool bUseLeftHandIK = true;
};
