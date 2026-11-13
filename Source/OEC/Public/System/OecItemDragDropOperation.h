// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "OecItemDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	// 드래그 중인 아이템 코드
	UPROPERTY(BlueprintReadWrite)
	FName ItemCode;

	// 시작된 슬롯 번호 (인벤토리용)
	UPROPERTY(BlueprintReadWrite)
	int32 SourceSlotIndex;

	// 인벤토리에서 왔는지, 퀵슬롯에서 왔는지 구분용
	UPROPERTY(BlueprintReadWrite)
	bool bFromInventory;

};
