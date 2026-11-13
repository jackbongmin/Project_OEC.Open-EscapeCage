// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/OecCharacterBase.h"
#include "Interfaces/OecPoolableInterface.h"
#include "OecMonsterBase.generated.h"

/**
 * 
 */
class UOecAttributeSetBase;
struct FOnAttributeChangeData;

UCLASS()
class OEC_API AOecMonsterBase : public AOecCharacterBase, public IOecPoolableInterface
{
	GENERATED_BODY()
	
public:
	AOecMonsterBase();

	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

protected:
	virtual void BeginPlay() override;

	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);

	void Die();

	void ReturnToPoolTimer();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOecAttributeSetBase> AttributeSet;

	bool bIsDead = false;

	FTimerHandle DeathTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "OEC|Monster")
	FName GetMonsterCode() const { return MonsterCode; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Monster")
	FName MonsterCode;

};
