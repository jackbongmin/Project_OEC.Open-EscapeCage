// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OecInteractRangeComponent.h"

UOecInteractRangeComponent::UOecInteractRangeComponent()
{
	InitSphereRadius(150.0f);

	// 플레이어 감지 전용: 다른 채널은 전부 무시하고 Pawn과만 겹침
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionObjectType(ECC_WorldDynamic);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetGenerateOverlapEvents(true);

	SetCanEverAffectNavigation(false);
	CanCharacterStepUpOn = ECB_No;
	SetHiddenInGame(true);
}
