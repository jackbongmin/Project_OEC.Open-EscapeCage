// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "OecRoomEvent_PressurePlates.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * 압력판 퍼즐: 압력판 3개가 순서대로 깜빡인 뒤, 그 순서대로 밟으면 성공. 틀리면 즉시 실패
 * - 가이드라인 원안은 "단말기에 나온 순서"이지만, 단말기 연동 전까지는 판이 직접 깜빡여 순서를 보여 준다
 */
UCLASS()
class OEC_API AOecRoomEvent_PressurePlates : public AOecRoomEventBase
{
	GENERATED_BODY()

public:
	AOecRoomEvent_PressurePlates();

protected:
	virtual void OnEventStarted() override;
	virtual void OnEventFinished(bool bInSuccess) override;

	UFUNCTION()
	void OnPlateBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult);

	// 순서 보여 주기 연출용 (몇 번째 판이 켜졌는지)
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Event")
	void BP_OnPlateShown(int32 InPlateIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TArray<TObjectPtr<UStaticMeshComponent>> PlateMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TArray<TObjectPtr<UBoxComponent>> PlateTriggers;

	// 판 하나가 켜져 있는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event", meta = (ClampMin = "0.1"))
	float ShowStepTime = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor IdleColor = FLinearColor(0.25f, 0.25f, 0.28f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor ShowColor = FLinearColor(1.0f, 0.8f, 0.1f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor CorrectColor = FLinearColor(0.1f, 0.9f, 0.3f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Event")
	FLinearColor WrongColor = FLinearColor(1.0f, 0.1f, 0.1f);

private:
	void ShowNextStep();

	void SetAllPlateColors(const FLinearColor& InColor);

private:
	// 밟아야 하는 순서 (판 번호)
	TArray<int32> Sequence;

	// 보여 주는 중이면 다음에 보여 줄 순서, 입력 중이면 다음에 밟아야 할 순서
	int32 StepIndex = 0;

	bool bAcceptingInput = false;

	FTimerHandle ShowTimerHandle;
};
