// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/Item/OecItemPickupBase.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AOecItemPickupBase::AOecItemPickupBase()
{
	// 떨어지는 동안만 Tick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ItemCode = NAME_None;
	Quantity = 1;
}

void AOecItemPickupBase::SetItemData(FName InItemCode, int32 InQuantity)
{
	ItemCode = InItemCode;
	Quantity = FMath::Max(InQuantity, 1);
}

void AOecItemPickupBase::StartDrop(float InGroundZ, USoundBase* InLandSound)
{
	// 메시 피벗이 가운데여도 바운드 아래쪽이 바닥에 닿는 높이에서 멈추도록 계산
	FVector boundsOrigin;
	FVector boundsExtent;
	GetActorBounds(true, boundsOrigin, boundsExtent);
	const float bottomOffset = GetActorLocation().Z - (boundsOrigin.Z - boundsExtent.Z);

	LandActorZ = InGroundZ + bottomOffset;
	PendingLandSound = InLandSound;
	DropSpeed = 0.0f;

	SetActorTickEnabled(true);
}

void AOecItemPickupBase::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);

	DropSpeed += DropGravity * InDeltaTime;
	FVector location = GetActorLocation();
	location.Z -= DropSpeed * InDeltaTime;

	if (location.Z > LandActorZ)
	{
		SetActorLocation(location);
		return;
	}

	// 착지
	location.Z = LandActorZ;
	SetActorLocation(location);
	SetActorTickEnabled(false);

	if (PendingLandSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PendingLandSound, location);
	}

	BP_OnLanded();
}

void AOecItemPickupBase::Interact_Implementation(AActor* InInteractor)
{
	Super::Interact_Implementation(InInteractor);

	if (ItemCode == NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 아이템 코드가 세팅되지 않았습니다!"), *GetName());
		return;
	}

	UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
	if (!invenSub) return;

	// 데이터 테이블에 없는 코드면 인벤토리 가득 참과 구분해서 알림
	const UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if (!dataSub || !dataSub->GetItemData(ItemCode))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] 아이템 데이터 테이블에 [%s] 행이 없습니다. 행 이름(Row Name)이 아이템 코드와 같은지 확인하세요"), *GetName(), *ItemCode.ToString());
		return;
	}

	bool bSuccess = invenSub->TryAddItem(ItemCode, Quantity);

	if (bSuccess)
	{
		SetInteractPromptVisible(false);
		UE_LOG(LogTemp, Log, TEXT("[%s] 아이템 획득 성공: %d개"), *ItemCode.ToString(), Quantity);

		Destroy(); // 냠냠
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("인벤토리가 가득 차서 [%s]을(를) 획득할 수 없습니다."), *ItemCode.ToString());
	}
}
