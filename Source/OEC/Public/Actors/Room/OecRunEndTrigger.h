// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/OecEnumType.h"
#include "OecRunEndTrigger.generated.h"

class UBoxComponent;

/**
 * 회차 종료 트리거 (탈출 방 출구 등)
 *
 * - 플레이어가 상자 영역에 들어오면 EndReason으로 회차를 끝낸다. 종료 화면은 HUD가 띄운다.
 * - 기본값은 탈출(노말 엔딩). 나중에 진엔딩용 숨겨진 문 같은 곳에도 사유만 바꿔 재사용한다.
 * - 문이 열리는 연출 등은 BP_OnTriggered에서 블루프린트로 처리한다.
 */
UCLASS()
class OEC_API AOecRunEndTrigger : public AActor
{
	GENERATED_BODY()

public:
	AOecRunEndTrigger();

	// 회차가 끝나기 직전에 호출 (연출용)
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Run")
	void BP_OnTriggered(AActor* InPlayer);

protected:
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	// 이 트리거로 끝날 때의 사유
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Run")
	EOecRunEndReason EndReason = EOecRunEndReason::Escape;
};
