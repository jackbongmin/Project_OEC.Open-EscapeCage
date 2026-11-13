// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "GAS/OecGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/OecInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Subsystems/OecQuickSlotSubsystem.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Core/OecPlayerController.h"
#include "Core/OecHUD.h"
#include "UI/Ingame/OecInGameWidget.h"
#include "UI/Ingame/OecInventoryPanelWidget.h"
#include "Data/OecEnumType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Attributeset/OecPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"

#include "Actors/Item/Weapon/OecWeaponBase.h"
#include "Actors/Item/Weapon/OecRangedWeapon.h"
#include "Data/OecDataStruct.h"

#include "GameplayAbilitySpec.h"
#include "GAS/Ability/OecGameplayAbility.h"

#include "UI/Ingame/OecInGameWidget.h"
#include "UI/Ingame/OecStatPanelWidget.h" 
#include "UI/Ingame/OecWeaponPanelWidget.h"


AOecPlayerCharacter::AOecPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    InteractionComponent = CreateDefaultSubobject<UOecInteractionComponent>(TEXT("InteractionComponent"));

    PlayerAttributeSet = CreateDefaultSubobject<UOecPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
    GetCharacterMovement()->AirControl = 0.5f;
}

void AOecPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* playerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
    if (UAbilitySystemComponent* asc = GetAbilitySystemComponent())
    {
        PlayerAttributeSet = const_cast<UOecPlayerAttributeSet*>(asc->GetSet<UOecPlayerAttributeSet>());

        asc->AddLooseGameplayTag(OecGameplayTags::Character_Player);
        if (HasAuthority())
        {
            for (TSubclassOf<UOecGameplayAbility>& AbilityClass : DefaultAbilities)
            {
                if (AbilityClass)
                {
                    asc->GiveAbility(FGameplayAbilitySpec(
                        AbilityClass,
                        1,
                        static_cast<int32>(AbilityClass.GetDefaultObject()->AbilityInputID),
                        this));
                }
            }
        }
    }
    if (FirstPersonCamera)
    {
        DefaultCameraTransform = FirstPersonCamera->GetRelativeTransform();
    }
}

void AOecPlayerCharacter::Tick(float InDeltaTime)
{
    Super::Tick(InDeltaTime);

    HandleStamina(InDeltaTime);

    UpdateMovementSettings();

    if (FirstPersonCamera)
    {
        float TargetFOV = bIsAiming ? AimFOV : DefaultFOV;
        float CurrentFOV = FirstPersonCamera->FieldOfView;

        // ==========================================
        // 1. 카메라 이동 및 회전 보간
        // ==========================================
        if (bIsAiming && CurrentWeaponActor && CurrentWeaponActor->GetWeaponMesh())
        {
            // 💡 [조준 중] 절대적인 '월드 좌표' 기준으로 조준경에 카메라를 갖다 붙임!
            FVector TargetWorldLoc = CurrentWeaponActor->GetWeaponMesh()->GetSocketLocation(TEXT("SightSocket"));
            FRotator TargetWorldRot = CurrentWeaponActor->GetWeaponMesh()->GetSocketRotation(TEXT("SightSocket"));

            FVector CurrentWorldLoc = FirstPersonCamera->GetComponentLocation();
            FRotator CurrentWorldRot = FirstPersonCamera->GetComponentRotation();

            FVector NewLoc = FMath::VInterpTo(CurrentWorldLoc, TargetWorldLoc, InDeltaTime, ZoomInterpSpeed);
            FRotator NewRot = FMath::RInterpTo(CurrentWorldRot, TargetWorldRot, InDeltaTime, ZoomInterpSpeed);

            FirstPersonCamera->SetWorldLocationAndRotation(NewLoc, NewRot);
        }
        else
        {
            // 💡 [비조준 중] 캡슐을 기준으로 한 원래 얼굴 위치 '상대 좌표'로 복귀!
            FVector TargetRelLoc = DefaultCameraTransform.GetLocation();
            FRotator TargetRelRot = DefaultCameraTransform.GetRotation().Rotator();

            FVector CurrentRelLoc = FirstPersonCamera->GetRelativeLocation();
            FRotator CurrentRelRot = FirstPersonCamera->GetRelativeRotation();

            FVector NewLoc = FMath::VInterpTo(CurrentRelLoc, TargetRelLoc, InDeltaTime, ZoomInterpSpeed);
            FRotator NewRot = FMath::RInterpTo(CurrentRelRot, TargetRelRot, InDeltaTime, ZoomInterpSpeed);

            FirstPersonCamera->SetRelativeLocationAndRotation(NewLoc, NewRot);
        }

        // ==========================================
        // 2. FOV (시야각) 보간 적용
        // ==========================================
        float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, InDeltaTime, ZoomInterpSpeed);
        FirstPersonCamera->SetFieldOfView(NewFOV);
    }
    UpdateStatUI();
    UpdateWeaponAmmoUI();
}

void AOecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent)
{
    Super::SetupPlayerInputComponent(InPlayerInputComponent);

    if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InPlayerInputComponent))
    {
        enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOecPlayerCharacter::Move);
        enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOecPlayerCharacter::Look);
        enhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::Interact);
        enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnSprintStarted);
        enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AOecPlayerCharacter::OnSprintCompleted);
        enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnJumpAction);

        if (QuickSlot1Action)
        {
            enhancedInputComponent->BindAction(QuickSlot1Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 1);
            enhancedInputComponent->BindAction(QuickSlot2Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 2);
            enhancedInputComponent->BindAction(QuickSlot3Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 3);
            enhancedInputComponent->BindAction(QuickSlot4Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 4);
            enhancedInputComponent->BindAction(QuickSlot5Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 5);
        }
        if (ToggleInventoryAction)
        {
            enhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::ToggleInventory);
        }
        if (FireAction)
        {
            enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnFireStarted);
            enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AOecPlayerCharacter::OnFireCompleted);
        }
        if (AimAction)
        {
            enhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnAimStarted);
            enhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AOecPlayerCharacter::OnAimCompleted);
        }
        if (ReloadAction)
        {
            enhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnReloadStarted);
        }
    }
}

void AOecPlayerCharacter::Move(const FInputActionValue& InValue)
{
    FVector2D movementVector = InValue.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddMovementInput(GetActorForwardVector(), movementVector.Y);
        AddMovementInput(GetActorRightVector(), movementVector.X);
    }
}

void AOecPlayerCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D lookAxisVector = InValue.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(lookAxisVector.X);
        AddControllerPitchInput(lookAxisVector.Y);
    }
}

void AOecPlayerCharacter::Interact()
{
    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}

void AOecPlayerCharacter::ToggleInventory()
{
    AOecPlayerController* pc = Cast<AOecPlayerController>(GetController());
    if (!pc) return;

    AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD());
    if (!hud || !hud->GetInGameWidget()) return;

    UOecInventoryPanelWidget* invenPanel = hud->GetInGameWidget()->GetInventoryPanel();
    if (!invenPanel) return;

    bool bIsVisible = invenPanel->GetVisibility() == ESlateVisibility::Visible;

    if (bIsVisible)
    {
        invenPanel->HideWidget();
        pc->SetUIInputMode(false); 
    }
    else
    {
        invenPanel->ShowWidget(); 
        pc->SetUIInputMode(true); 
    }
}

void AOecPlayerCharacter::HandleStamina(float InDeltaTime)
{
    if (!PlayerAttributeSet) return;

    float curStamina = PlayerAttributeSet->GetStamina();
    float maxStamina = PlayerAttributeSet->GetMaxStamina();

    float newStamina = curStamina;

    if (bIsSprinting && GetVelocity().Size() > 10.f && !bIsExhausted)
    {
        newStamina -= 10.f * InDeltaTime;
        RegenDelayTimer = 0.f;

        if (newStamina <= 0.f)
        {
            newStamina = 0.f;
            bIsExhausted = true;
            RegenDelayTimer = 5.f; 
        }
    }
    else
    {
        if (RegenDelayTimer > 0.f)
        {
            RegenDelayTimer -= InDeltaTime;
        }
        else
        {
            bIsExhausted = false;
            if (newStamina < maxStamina)
            {
                newStamina += 15.f * InDeltaTime;
            }
        }
    }

    if (newStamina != curStamina)
    {
        PlayerAttributeSet->SetStamina(FMath::Clamp(newStamina, 0.f, maxStamina));
    }
}

void AOecPlayerCharacter::UpdateMovementSettings()
{
    if (!PlayerAttributeSet)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("어트리뷰트 셋을 못 찾음"));
        return;
    }
    float walkSpeed = PlayerAttributeSet->GetSpeed();
    float sprintSpeed = PlayerAttributeSet->GetSprintSpeed();

    bool bCanSprint = bIsSprinting && !bIsExhausted;
    GetCharacterMovement()->MaxWalkSpeed = bCanSprint ? sprintSpeed : walkSpeed;

    GetCharacterMovement()->JumpZVelocity = PlayerAttributeSet->GetJumpForce();
    GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Walk: %f | Sprint: %f | IsSprinting: %d | bIsExhausted: %d"), walkSpeed, sprintSpeed, bIsSprinting, bIsExhausted));
}

void AOecPlayerCharacter::OnSprintStarted()
{
    bIsSprinting = true;
}

void AOecPlayerCharacter::OnSprintCompleted()
{
    bIsSprinting = false;
}

void AOecPlayerCharacter::OnJumpAction()
{
	Jump();
}

void AOecPlayerCharacter::OnFireStarted()
{
    if (CurrentWeaponActor)
    {
        CurrentWeaponActor->StartAttack();
    }

    if (APlayerController* pc = Cast<APlayerController>(GetController()))
    {
        if (AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD()))
        {
            if (hud->GetInGameWidget())
            {
                hud->GetInGameWidget()->PlayCrosshairFireAnim();
            }
        }
    }
}

void AOecPlayerCharacter::OnFireCompleted()
{
    if (CurrentWeaponActor)
    {
        CurrentWeaponActor->StopAttack();
    }

}

void AOecPlayerCharacter::OnReloadStarted()
{
    if (UAbilitySystemComponent* asc = GetAbilitySystemComponent())
    {
        asc->AbilityLocalInputPressed(static_cast<int32>(EOecAbilityInputID::Reload));
    }
}

void AOecPlayerCharacter::ExecuteQuickSlot(int32 InSlotIndex)
{
    UOecQuickSlotSubsystem* quickSlotSub = GetGameInstance()->GetSubsystem<UOecQuickSlotSubsystem>();
    UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
    UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();

    if (!quickSlotSub || !invenSub || !dataSub) return;

    // 해당 슬롯의 아이템 코드 가져오기
    FName targetItemCode = quickSlotSub->GetItemAtSlot(InSlotIndex);
    if (targetItemCode == NAME_None) return;

    int32 currentItemCount = invenSub->GetItemQuantity(targetItemCode);

    if (currentItemCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ 삐빅! 인벤토리에 아이템이 없습니다! (개수: 0)"));

        quickSlotSub->assignQuickSlot(InSlotIndex, NAME_None);

        return; 
    }

    // 데이터 테이블에서 상세 정보 찾기
    const FItemStaticData* itemData = dataSub->GetItemData(targetItemCode);
    if (!itemData) return;

    // 타입에 따라 행동 결정
    if (itemData->ItemType == EItemType::Consumable)
    {
        UE_LOG(LogTemp, Log, TEXT("%d번 슬롯 소비템 사용 시도: %s"), InSlotIndex, *itemData->ItemName);

        if (UAbilitySystemComponent* asc = GetAbilitySystemComponent())
        {
            asc->AbilityLocalInputPressed(static_cast<int32>(EOecAbilityInputID::ItemUse));

            invenSub->RemoveItem(targetItemCode, 1);
        }
    }
    else if (itemData->ItemType == EItemType::Weapon)
    {
        // 무기: 장착 프로세스 시작!
        UE_LOG(LogTemp, Log, TEXT("%d번 슬롯 무기 장착: %s"), InSlotIndex, *itemData->ItemName);

        // 데이터 테이블에 적힌 애니메이션 상태(Rifle, Pistol 등)와 아이템 코드를 넘김
        SetWeaponState(itemData->WeaponAnimState, targetItemCode);

        if (APlayerController* pc = Cast<APlayerController>(GetController()))
        {
            if (AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD()))
            {
                if (hud->GetInGameWidget())
                {
                    hud->GetInGameWidget()->UpdateCrosshairState(true);
                }
            }
        }
    }
}

void AOecPlayerCharacter::SetWeaponState(EOecWeaponState InNewState, FName InItemCode)
{
    UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 1. 함수 진입! 상태: %d, 아이템코드: %s"), (int32)InNewState, *InItemCode.ToString());

    CurrentWeaponState = InNewState;
    
     // 기존에 들고 있던 무기가 있다면 파괴 (또는 풀링 반납)
    if (CurrentWeaponActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 1-1. 기존 무기 파괴함"));
        CurrentWeaponActor->Destroy();
        CurrentWeaponActor = nullptr;
    }
    // 맨손이면 여기서 종료
    if (CurrentWeaponState == EOecWeaponState::None || InItemCode == NAME_None)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 2. 맨손 상태이거나 아이템 코드가 없어서 취소됨!"));
        return;
    }

    // 게임 데이터 서브시스템에서 아이템 정보(데이터 테이블) 긁어오기
    UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
    if (!dataSub)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 3. 데이터 서브시스템을 찾을 수 없음!"));
        return;
    }

    const FItemStaticData* itemData = dataSub->GetItemData(InItemCode);
    if (!itemData)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 4. 데이터 테이블에서 해당 아이템을 찾을 수 없음!"));
        return;
    }

    // 클래스 유효성 확인 (가장 의심되는 부분!!)
    if (itemData->ItemActorClass.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 5. 데이터 테이블에 스폰할 무기 클래스(ItemActorClass)가 비어있음!"));
        return;
    }

    UClass* WeaponClassToSpawn = itemData->ItemActorClass.LoadSynchronous();
    if (!WeaponClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 6. 무기 클래스를 메모리에 로드하는데 실패함!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 7. 무기 스폰 직전! 클래스: %s"), *WeaponClassToSpawn->GetName());

    FActorSpawnParameters spawnParams;
    spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AOecWeaponBase* spawnedWeapon = GetWorld()->SpawnActor<AOecWeaponBase>(WeaponClassToSpawn, GetActorLocation(), GetActorRotation(), spawnParams);

    if (spawnedWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 8. 스폰 성공! 데이터 주입 및 장착 진행"));
        spawnedWeapon->InitWeaponData(*itemData);
        spawnedWeapon->Equip(this, TEXT("HandGrip_R"));
        CurrentWeaponActor = spawnedWeapon;

        if (APlayerController* pc = Cast<APlayerController>(GetController()))
        {
            if (AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD()))
            {
                if (UOecInGameWidget* mainWidget = hud->GetInGameWidget())
                {
                    if (UOecWeaponPanelWidget* weaponPanel = mainWidget->GetWeaponPanel())
                    {
                        weaponPanel->UpdateIcon(itemData->ItemIcon);
                    }
                }
            }
        }

        UpdateWeaponAmmoUI();

        if (UAbilitySystemComponent* asc = GetAbilitySystemComponent())
        {
            if (itemData->WeaponAbilityClass)
            {
                FGameplayAbilitySpec spec(itemData->WeaponAbilityClass, 1, static_cast<int32>(EOecAbilityInputID::Fire), spawnedWeapon);

                asc->GiveAbility(spec);
                UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 무기 전용 발사 스킬(GA) 부여 완료!"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 에러! DT에 WeaponAbilityClass가 비어있음!"));
            }
            if (itemData->ReloadAbilityClass)
            {
                FGameplayAbilitySpec reloadSpec(itemData->ReloadAbilityClass, 1, static_cast<int32>(EOecAbilityInputID::Reload), spawnedWeapon);
                asc->GiveAbility(reloadSpec);
                UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 무기 전용 장전 스킬(GA) 부여 완료!"));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 9. SpawnActor 자체가 실패함!"));
    }
}

void AOecPlayerCharacter::OnAimStarted()
{
    // 현재 무기를 들고 있을 때만 조준 가능하게 처리! (맨손일 때 줌 땡기면 이상하니까)
    if (CurrentWeaponActor)
    {
        bIsAiming = true;
        if (APlayerController* pc = Cast<APlayerController>(GetController()))
            if (AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD()))
                if (hud->GetInGameWidget())
                    hud->GetInGameWidget()->UpdateCrosshairAimState(true);
    }
}

void AOecPlayerCharacter::OnAimCompleted()
{
    bIsAiming = false;
    if (APlayerController* pc = Cast<APlayerController>(GetController()))
        if (AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD()))
            if (hud->GetInGameWidget())
                hud->GetInGameWidget()->UpdateCrosshairAimState(false);
}

void AOecPlayerCharacter::UpdateStatUI()
{
    if (APlayerController* pc = Cast<APlayerController>(GetController()))
    {
        if (AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD()))
        {
            if (UOecInGameWidget* mainWidget = hud->GetInGameWidget())
            {
                if (UOecStatPanelWidget* statPanel = mainWidget->GetStatPanel())
                {
                    if (PlayerAttributeSet)
                    {
                        statPanel->UpdateHealth(PlayerAttributeSet->GetHealth(), PlayerAttributeSet->GetMaxHealth());
                        statPanel->UpdateStamina(PlayerAttributeSet->GetStamina(), PlayerAttributeSet->GetMaxStamina());
                        statPanel->UpdateSanity(PlayerAttributeSet->GetSanity(), PlayerAttributeSet->GetMaxSanity());
                    }
                }
            }
        }
    }
}

void AOecPlayerCharacter::UpdateWeaponAmmoUI()
{
    if (!CurrentWeaponActor) return;

    if (AOecRangedWeapon* rangedWeapon = Cast<AOecRangedWeapon>(CurrentWeaponActor))
    {
        if (APlayerController* pc = Cast<APlayerController>(GetController()))
        {
            if (AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD()))
            {
                if (UOecInGameWidget* mainWidget = hud->GetInGameWidget())
                {
                    if (UOecWeaponPanelWidget* weaponPanel = mainWidget->GetWeaponPanel())
                    {
                        UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
                        int32 reserveAmmo = invenSub ? invenSub->GetItemQuantity(rangedWeapon->GetUseAmmoItemCode()) : 0;

                        weaponPanel->UpdateAmmo(rangedWeapon->GetCurrentAmmo(), reserveAmmo);
                    }
                }
            }
        }
    }
}
