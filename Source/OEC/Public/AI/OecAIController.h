// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OecAIController.generated.h"

/**
 *
 */

class UAISenseConfig_Sight;
struct FAIStimulus;
class AOecMonsterBase;

UCLASS()
class OEC_API AOecAIController : public AAIController
{
	GENERATED_BODY()

public:
	AOecAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	// 몬스터 사망·풀 반납(false) / 풀에서 다시 꺼냄(true)
	UFUNCTION()
	void OnMonsterActiveChanged(AOecMonsterBase* InMonster, bool bInActive);

	// 시야 감지, BT, 디버그 시야 표시를 한 번에 켜고 끔
	void SetPerceptionActive(bool bInActive);

private:
	bool bIsSeeingTarget = false;

	// 시야 감지가 켜져 있는지 (사망하면 false)
	bool bIsPerceptionActive = true;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// 테스트용 시야 원뿔 표시 (끄면 Tick도 꺼짐)
	UPROPERTY(EditAnywhere, Category = "OEC|AI|Debug")
	bool bDrawSightDebug = true;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

};
