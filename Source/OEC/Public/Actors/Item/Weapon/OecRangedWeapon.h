// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Item/Weapon/OecWeaponBase.h"
#include "OecRangedWeapon.generated.h"

/**
 * 
 */

class UNiagaraSystem;
class USoundBase;
class UNiagaraComponent;
class UGameplayEffect;

UCLASS()
class OEC_API AOecRangedWeapon : public AOecWeaponBase
{
	GENERATED_BODY()

public:
	AOecRangedWeapon();

	virtual void InitWeaponData(const FItemStaticData& InWeaponData) override;

protected:
	/* ==========================================
	   총기 핵심 스탯
	========================================== */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Stats")
	int32 MaxAmmoInClip = 30; // 1탄창 최대치

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Weapon|Stats")
	int32 CurrentAmmo = 30; // 현재 탄창 잔탄

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Stats")
	float FireRate = 0.1f; // 연사 속도 (0.1초마다 발사)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Stats")
	float ZoomFOV = 45.0f; // 정조준 시 카메라 시야각(FOV)

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Stats")
	float FireDistance = 10000.0f; // 총알이 날아가는 최대 거리 (100m)

	/* ==========================================
	   VFX (시각 효과) & SFX (사운드)
	========================================== */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TObjectPtr<UNiagaraSystem> MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TObjectPtr<UNiagaraSystem> ImpactFX; // 벽/적 피격 스파크

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TObjectPtr<USoundBase> FireSound; // 탕! 소리

	// 연사 타이머 관리용
	FTimerHandle FireTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
public:
	// 마우스 좌클릭 (총알 발사 시작)
	virtual void StartAttack() override;
	// 마우스 좌클릭 뗌 (연사 중지)
	virtual void StopAttack() override;

	// 정조준 (마우스 우클릭)
	UFUNCTION(BlueprintCallable, Category = "OEC|Weapon|Ranged")
	virtual void Zoom(bool bInIsZooming);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OEC|Weapon|Stats")
	float SpreadAngle = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Weapon|Effects")
	TObjectPtr<UNiagaraSystem> TracerFX;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|Feedback")
	TSubclassOf<class UCameraShakeBase> FireCameraShakeClass;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|Feedback")
	float MinPitchRecoil = -0.6f;

	UPROPERTY(EditDefaultsOnly, Category = "OEC|Feedback")
	float MaxPitchRecoil = -0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "OEC|Feedback")
	float YawRecoilRange = 0.5f;

public:
	void PlayFireFX();

	int32 GetCurrentAmmo() const { return CurrentAmmo; }
	void ConsumeAmmo(int32 InAmount) { CurrentAmmo = FMath::Max(0, CurrentAmmo - InAmount); }

public:
	float GetFireDistance() const { return FireDistance; }
	float GetSpreadAngle() const { return SpreadAngle; }

protected:
	UPROPERTY()
	FName UseAmmoItemCode;

	UPROPERTY()
	UAnimMontage* ReloadMontage;

public:
	int32 GetMaxAmmoInClip() const { return MaxAmmoInClip; }
	FName GetUseAmmoItemCode() const { return UseAmmoItemCode; }
	UAnimMontage* GetReloadMontage() const { return ReloadMontage; }

	void AddAmmo(int32 InAmount) { CurrentAmmo = FMath::Min(MaxAmmoInClip, CurrentAmmo + InAmount); }

public:
	void PlayHitAndTracerFX(const FHitResult& InHitResult, const FVector& InTraceEnd, bool InbHit);

	void TriggerFireAbility();

};
