// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/OecCharacterBase.h"
#include "Data/OecEnumType.h"
#include "OecPlayerCharacter.generated.h"

/**
 * 
 */

class UCameraComponent;
class UOecInteractionComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
enum class EOecWeaponState : uint8;
class UOecPlayerAttributeSet;
class UOecCrosshairWidget;

UCLASS()
class OEC_API AOecPlayerCharacter : public AOecCharacterBase
{
	GENERATED_BODY()
	
public:
	AOecPlayerCharacter();

protected:
	virtual void BeginPlay() override;

    virtual void Tick(float InDeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Enhanced Input
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();

	void ToggleInventory();

    void HandleStamina(float InDeltaTime);
    void UpdateMovementSettings();

public:
    void OnSprintStarted();
    void OnSprintCompleted();
    void OnJumpAction();

    void OnFireStarted(); 
    void OnFireCompleted();

    void OnReloadStarted();

protected:
    // 1인칭 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCamera;

    // 상호작용 (레이저 쏘기)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Interaction", meta = (AllowPrivateAccess = "true"))
    UOecInteractionComponent* InteractionComponent;

    // Enhanced Input 에셋 슬롯
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Input")
    TObjectPtr<UInputAction> FireAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Input")
    TObjectPtr<UInputAction> ReloadAction;



public:
    void ExecuteQuickSlot(int32 InSlotIndex);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* ToggleInventoryAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot1Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot2Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot3Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot4Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot5Action;


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Weapon")
    EOecWeaponState CurrentWeaponState = EOecWeaponState::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Weapon")
    TObjectPtr<class AOecWeaponBase> CurrentWeaponActor;

    UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
    void SetWeaponState(EOecWeaponState InNewState, FName InItemCode);

    UFUNCTION(BlueprintCallable, Category = "OEC|Weapon")
	AOecWeaponBase* GetCurrentWeapon() const { return CurrentWeaponActor; }

protected:
    UPROPERTY()
    TObjectPtr<UOecPlayerAttributeSet> PlayerAttributeSet;

protected:
    bool bIsSprinting = false;
    bool bIsExhausted = false;
    float RegenDelayTimer = 0.f;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "OEC|UI")
    TSubclassOf<UOecCrosshairWidget> CrosshairWidgetClass;

    UPROPERTY()
    TObjectPtr<UOecCrosshairWidget> CrosshairWidgetInstance;

public:
    void SetAiming(bool bInAiming) { bIsAiming = bInAiming; }

protected:
    // 현재 정조준 중인지 체크
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Camera")
    bool bIsAiming = false;

    // 기본 시야각 (보통 90)
    UPROPERTY(EditDefaultsOnly, Category = "OEC|Camera")
    float DefaultFOV = 90.0f;

    // 정조준 시 시야각 (숫자가 작아질수록 화면이 확대됨!)
    UPROPERTY(EditDefaultsOnly, Category = "OEC|Camera")
    float AimFOV = 60.0f;

    // 줌인/줌아웃 속도
    UPROPERTY(EditDefaultsOnly, Category = "OEC|Camera")
    float ZoomInterpSpeed = 15.0f;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    class UInputAction* AimAction;

    void OnAimStarted();
    void OnAimCompleted();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Camera")
    FTransform DefaultCameraTransform;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Abilities")
    TArray<TSubclassOf<class UOecGameplayAbility>> DefaultAbilities;

public:
    UFUNCTION(BlueprintCallable, Category = "OEC|UI")
    void UpdateStatUI();

    UFUNCTION(BlueprintCallable, Category = "OEC|UI")
    void UpdateWeaponAmmoUI();

};
