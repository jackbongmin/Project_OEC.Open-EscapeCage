// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OecQuickSlotSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickSlotUpdated);

UCLASS()
class OEC_API UOecQuickSlotSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category = "OEC|QuickSlot")
	void assignQuickSlot(int32 InSlotIndex, FName InItemID);
	
	UFUNCTION(BlueprintCallable, Category = "OEC|QuickSlot")
	void ClearQuickSlot(int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "OEC|QuickSlot")
	FName GetItemAtSlot(int32 InSlotIndex) const;

	UPROPERTY(BlueprintAssignable, Category = "OEC|QuickSlot")
	FOnQuickSlotUpdated OnQuickSlotUpdated;

private:
	UPROPERTY()
	TMap<int32, FName> QuickSlots;

};
