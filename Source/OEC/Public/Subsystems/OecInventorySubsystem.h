// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/OecDataStruct.h"

#include "OecInventorySubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS()
class OEC_API UOecInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "OEC|Inventory")
    bool TryAddItem(FName InItemCode, int32 InCount);

    UFUNCTION(BlueprintCallable, Category = "OEC|Inventory")
    void RemoveItem(FName InItemCode, int32 InCount);

    UFUNCTION(BlueprintCallable, Category = "OEC|Inventory")
    void SwapSlot(int32 InSourceIndex, int32 InDestinationIndex);

    UFUNCTION(BlueprintPure, Category = "OEC|Inventory")
    const TArray<FInventorySlot>& GetSlots() const { return InventorySlots; }

    UPROPERTY(BlueprintAssignable, Category = "OEC|Events")
    FOnInventoryUpdated OnInventoryUpdated;

private:
    UPROPERTY()
    TArray<FInventorySlot> InventorySlots;

    int32 MaxSlotCount = 20;

public:
    UFUNCTION(BlueprintCallable, Category = "OEC|Inventory")
    int32 GetItemQuantity(FName InItemCode) const;
};
