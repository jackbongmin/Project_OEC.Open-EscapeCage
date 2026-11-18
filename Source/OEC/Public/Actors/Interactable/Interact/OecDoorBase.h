// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable/OecInteractableBase.h"
#include "Data/OecEnumType.h"
#include "OecDoorBase.generated.h"

/**
 * 열쇠로 여는 문 (메인 문, 복도의 랜덤 방 문)
 *
 * - 열쇠 아이템(프로젝트 설정 KeyItemCode)을 RequiredKeyCount개 가지고 E를 누르면 열쇠를 소모하고 열린다.
 * - 메인 문도 같은 방식: 페이즈 1을 끝내면 열쇠가 떨어지고, 플레이어가 직접 들고 와서 연다 (가짜 탈출의 허무함).
 * - 열쇠가 없을 때의 거절 소리·흔들림은 BP_OnOpenDenied에서 처리한다.
 */
UCLASS()
class OEC_API AOecDoorBase : public AOecInteractableBase
{
	GENERATED_BODY()

public:
	AOecDoorBase();

	UFUNCTION(BlueprintCallable, Category = "OEC|Door")
	void SetDoorState(bool bInOpen);

    UFUNCTION(BlueprintCallable, Category = "OEC|Door")
    void SetDoorLocked(bool bInLocked);

	virtual void Interact_Implementation(AActor* InInteractor) override;
	virtual bool CanInteract_Implementation(AActor* InInteractor) const override;

protected:
    // 문 종류 (연출 구분용. 여는 규칙은 같다)
    UPROPERTY(EditAnywhere, Category = "OEC|Door")
    EDoorType DoorType;

    // 여는 데 필요한(소모하는) 열쇠 수
    UPROPERTY(EditAnywhere, Category = "OEC|Door", meta = (ClampMin = "0"))
    int32 RequiredKeyCount = 1;

    UPROPERTY(BlueprintReadOnly, Category = "OEC|Door")
    bool bIsOpen = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Door")
    bool bIsLocked = false;

    UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Door")
    void BP_OnDoorStateChanged(bool bInIsOpen);

    // 열쇠가 부족해서 열지 못했을 때 (거절 소리, 화면 흔들림 등)
    UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Door")
    void BP_OnOpenDenied(int32 InCurrentKeyCount, int32 InRequiredKeyCount);
};
