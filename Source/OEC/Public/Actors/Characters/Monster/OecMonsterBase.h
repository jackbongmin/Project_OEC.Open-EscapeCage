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
class AOecMonsterBase;

// 활동 상태 변경 (사망·풀 반납 = false, 풀에서 다시 꺼냄 = true). AI 컨트롤러가 구독해 시야·BT를 켜고 끈다
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMonsterActiveChangedDelegate, AOecMonsterBase*, InMonster, bool, bInActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDiedDelegate, AOecMonsterBase*, InMonster);

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

	// 활동 상태가 바뀔 때만 OnMonsterActiveChanged 발행
	void SetMonsterActive(bool bInActive);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOecAttributeSetBase> AttributeSet;

	bool bIsDead = false;

	// 살아 있고 월드에서 활동 중인지 (사망하거나 풀에 들어가면 false)
	bool bIsMonsterActive = true;

	FTimerHandle DeathTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "OEC|Monster")
	FName GetMonsterCode() const { return MonsterCode; }

	UFUNCTION(BlueprintPure, Category = "OEC|Monster")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category = "OEC|Monster")
	bool IsMonsterActive() const { return bIsMonsterActive; }

	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnMonsterActiveChangedDelegate OnMonsterActiveChanged;

	UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
	FOnMonsterDiedDelegate OnMonsterDied;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Monster")
	FName MonsterCode;

};
