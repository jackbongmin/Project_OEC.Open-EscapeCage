// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_SupplyDefense.h"
#include "GAS/OecGameplayTags.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AOecRoomEvent_SupplyDefense::AOecRoomEvent_SupplyDefense()
{
	CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh"));
	CrateMesh->SetupAttachment(RootComponent);
	CrateMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
	CrateMesh->SetRelativeScale3D(FVector(1.2f, 0.8f, 0.8f));
	CrateMesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (cubeMesh.Succeeded())
	{
		CrateMesh->SetStaticMesh(cubeMesh.Object);
	}

	// QuestionGuideline.md 5장: 생존 → 무기 R 또는 의료 키트
	RewardItemCodes = { TEXT("Rifle_R"), TEXT("Pistol_R"), TEXT("Heal_L") };

	// 열린 상자를 잠깐 보여 준 뒤 제거
	DestroyDelay = 3.0f;
}

void AOecRoomEvent_SupplyDefense::OnEventStarted()
{
	SetBasicShapeColor(CrateMesh, LockedColor);
}

void AOecRoomEvent_SupplyDefense::OnEventFinished(bool bInSuccess)
{
	if (!bInSuccess || RewardItemCodes.Num() == 0) return;

	SetBasicShapeColor(CrateMesh, OpenedColor);

	FOecChoiceEffect rewardEffect;
	rewardEffect.EffectTag = OecGameplayTags::Effect_Give_Item;
	rewardEffect.Value = 1.0f;
	rewardEffect.Param = RewardItemCodes[FMath::RandRange(0, RewardItemCodes.Num() - 1)];
	ExecuteEventEffect(rewardEffect);
}
