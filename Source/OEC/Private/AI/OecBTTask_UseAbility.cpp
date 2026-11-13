// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/OecBTTask_UseAbility.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Actors/Characters/Monster/OecMonsterBase.h"
#include "GAS/Ability/OecGameplayAbility.h"

UOecBTTask_UseAbility::UOecBTTask_UseAbility()
{
	NodeName = TEXT("Use GAS Ability");
}

EBTNodeResult::Type UOecBTTask_UseAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!AbilityToUse)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* aiController = OwnerComp.GetAIOwner();
	if (!aiController) return EBTNodeResult::Failed;

	APawn* pawn = aiController->GetPawn();
	if (!pawn) return EBTNodeResult::Failed;

	UAbilitySystemComponent* asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(pawn);
	if (asc)
	{
		bool bSuccess = asc->TryActivateAbilityByClass(AbilityToUse);

		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("👾 몬스터: 스킬 사용 성공!"));
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
