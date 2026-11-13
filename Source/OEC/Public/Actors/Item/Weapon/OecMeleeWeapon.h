// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Item/Weapon/OecWeaponBase.h"
#include "OecMeleeWeapon.generated.h"

/**
 * 
 */

class UBoxComponent;

UCLASS()
class OEC_API AOecMeleeWeapon : public AOecWeaponBase
{
	GENERATED_BODY()
	
public:
	AOecMeleeWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	UBoxComponent* HitCollision;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHitOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult);

public:
	virtual void StartAttack() override;
	virtual void StopAttack() override;

	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon|Melee")
	void EnableHitCollision();

	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon|Melee")
	void DisableHitCollision();
};
