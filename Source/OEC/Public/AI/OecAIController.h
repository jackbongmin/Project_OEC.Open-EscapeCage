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

private:
	bool bIsSeeingTarget = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	virtual void OnPossess(APawn* InPawn) override;

};
