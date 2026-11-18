// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_SupplyDefense.generated.h"

class UStaticMeshComponent;

/**
 * 보급 상자 방어: 잠긴 상자가 떨어지고 좀비가 몰려온다 (좀비는 데이터의 시작 효과). 끝까지 버티면 상자가 열려 보상 중 하나
 */
UCLASS()
class OEC_API AOecRoomEvent_SupplyDefense : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_SupplyDefense();

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> CrateMesh;

	// 성공 시 이 중 하나를 지급 (아이템 코드)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	TArray<FName> RewardItemCodes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor LockedColor = FLinearColor(0.6f, 0.45f, 0.05f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor OpenedColor = FLinearColor(0.1f, 0.8f, 0.3f);
};
