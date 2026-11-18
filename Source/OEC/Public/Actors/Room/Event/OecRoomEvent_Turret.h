// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "AbilitySystemInterface.h"
#include "OecRoomEvent_Turret.generated.h"

class UStaticMeshComponent;
class UAbilitySystemComponent;
class UOecAttributeSetBase;
struct FOnAttributeChangeData;

/**
 * 천장 터렛: 천장에서 플레이어를 조준해 사격한다. 엄폐 벽에 가려지면 맞지 않는다
 * - ASC와 체력 어트리뷰트를 가져서 기존 총·근접 무기의 피해 GE로 부술 수 있다. 부수면 성공 (CompleteEvent)
 * - 엄폐 벽은 데이터의 시작 효과(Effect.Event.Start Event_CoverWalls)로 함께 나온다
 */
UCLASS()
class OEC_API AOecRoomEvent_Turret : public AOecRoomEventBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOecRoomEvent_Turret();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	// 사격 연출용 (총구 위치, 끝 위치, 플레이어 명중 여부)
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Event")
	void BP_OnTurretFired(const FVector& InMuzzleLocation, const FVector& InEndLocation, bool bInHitPlayer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> TurretBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> TurretBarrel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UOecAttributeSetBase> AttributeSet;

	// 바닥 기준 터렛 높이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	float TurretHeight = 380.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "1"))
	float TurretHealth = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.1"))
	float FireInterval = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0"))
	float ShotDamage = 4.0f;

	// 사격 궤적 디버그 선 (개발 빌드에서만)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	bool bDrawShotDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor TurretColor = FLinearColor(0.5f, 0.05f, 0.05f);

private:
	void Fire();

	void OnTurretHealthChanged(const FOnAttributeChangeData& InData);

	FTimerHandle FireTimerHandle;
};
