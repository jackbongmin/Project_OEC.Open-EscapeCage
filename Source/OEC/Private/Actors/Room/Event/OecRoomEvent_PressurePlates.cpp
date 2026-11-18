// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_PressurePlates.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Algo/RandomShuffle.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

AOecRoomEvent_PressurePlates::AOecRoomEvent_PressurePlates()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	// 이벤트 기준점 둘레에 120도 간격으로 3개 (반경 400)
	for (int32 i = 0; i < 3; ++i)
	{
		const float angle = FMath::DegreesToRadians(90.0f + i * 120.0f);
		const FVector plateLocation(FMath::Cos(angle) * 400.0f, FMath::Sin(angle) * 400.0f, 0.0f);

		UStaticMeshComponent* plateMesh = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("PlateMesh%d"), i));
		plateMesh->SetupAttachment(RootComponent);
		plateMesh->SetRelativeLocation(plateLocation + FVector(0.0f, 0.0f, 2.0f));
		plateMesh->SetRelativeScale3D(FVector(1.6f, 1.6f, 0.04f));
		plateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (cylinderMesh.Succeeded())
		{
			plateMesh->SetStaticMesh(cylinderMesh.Object);
		}
		PlateMeshes.Add(plateMesh);

		UBoxComponent* plateTrigger = CreateDefaultSubobject<UBoxComponent>(*FString::Printf(TEXT("PlateTrigger%d"), i));
		plateTrigger->SetupAttachment(RootComponent);
		plateTrigger->SetRelativeLocation(plateLocation + FVector(0.0f, 0.0f, 40.0f));
		plateTrigger->SetBoxExtent(FVector(70.0f, 70.0f, 40.0f));
		plateTrigger->SetCollisionProfileName(TEXT("Trigger"));
		plateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AOecRoomEvent_PressurePlates::OnPlateBeginOverlap);
		PlateTriggers.Add(plateTrigger);
	}

	DestroyDelay = 1.5f;
}

void AOecRoomEvent_PressurePlates::OnEventStarted()
{
	// 1. 무작위 순서 정하기 → 하나씩 깜빡여 보여 주기
	Sequence = { 0, 1, 2 };
	Algo::RandomShuffle(Sequence);

	SetAllPlateColors(IdleColor);
	StepIndex = 0;
	bAcceptingInput = false;
	GetWorldTimerManager().SetTimer(ShowTimerHandle, this, &AOecRoomEvent_PressurePlates::ShowNextStep, ShowStepTime, true, 0.5f);
}

void AOecRoomEvent_PressurePlates::ShowNextStep()
{
	SetAllPlateColors(IdleColor);

	// 2. 다 보여 줬으면 입력 받기
	if (StepIndex >= Sequence.Num())
	{
		GetWorldTimerManager().ClearTimer(ShowTimerHandle);
		StepIndex = 0;
		bAcceptingInput = true;
		UE_LOG(LogTemp, Log, TEXT("[%s] 압력판 순서 표시 끝, 입력 시작"), *GetName());
		return;
	}

	const int32 plateIndex = Sequence[StepIndex++];
	SetBasicShapeColor(PlateMeshes[plateIndex], ShowColor);
	BP_OnPlateShown(plateIndex);
}

void AOecRoomEvent_PressurePlates::OnPlateBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult)
{
	const APawn* pawn = Cast<APawn>(InOtherActor);
	if (!bAcceptingInput || IsEventFinished() || !pawn || !pawn->IsPlayerControlled()) return;

	const int32 plateIndex = PlateTriggers.IndexOfByKey(InOverlappedComponent);
	if (plateIndex == INDEX_NONE) return;

	// 3. 순서 확인: 맞으면 초록, 틀리면 빨강 + 즉시 실패
	if (plateIndex != Sequence[StepIndex])
	{
		SetBasicShapeColor(PlateMeshes[plateIndex], WrongColor);
		CompleteEvent(false);
		return;
	}

	SetBasicShapeColor(PlateMeshes[plateIndex], CorrectColor);
	if (++StepIndex >= Sequence.Num())
	{
		CompleteEvent(true);
	}
}

void AOecRoomEvent_PressurePlates::SetAllPlateColors(const FLinearColor& InColor)
{
	for (UStaticMeshComponent* plateMesh : PlateMeshes)
	{
		SetBasicShapeColor(plateMesh, InColor);
	}
}

void AOecRoomEvent_PressurePlates::OnEventFinished(bool bInSuccess)
{
	GetWorldTimerManager().ClearTimer(ShowTimerHandle);
	bAcceptingInput = false;
}
