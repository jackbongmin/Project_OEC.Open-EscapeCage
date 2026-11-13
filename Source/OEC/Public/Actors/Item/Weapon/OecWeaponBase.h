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

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "OEC|Weapon")
	FTransform GetLeftGripTransform() const;

	float GetBaseDamage() const { return BaseDamage; }
};
