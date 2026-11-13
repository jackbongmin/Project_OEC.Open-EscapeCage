// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "OecBTTask_UseAbility.generated.h"

/**
 * 
 */

class UOecGameplayAbility;

UCLASS()
class OEC_API UOecBTTask_UseAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UOecBTTask_UseAbility();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "OEC|AI")
	TSubclassOf<UOecGameplayAbility> AbilityToUse;

};
