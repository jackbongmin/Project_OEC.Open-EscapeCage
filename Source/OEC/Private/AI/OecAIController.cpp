// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/OecAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

AOecAIController::AOecAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SigjtConfig"));

	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1200.0f; 
	SightConfig->PeripheralVisionAngleDegrees = 60.0f; 

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AOecAIController::BeginPlay()
{
	Super::BeginPlay();

	if (PerceptionComponent)
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AOecAIController::OnTargetDetected);
	}
}

void AOecAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	if (APawn* controlledPawn = GetPawn())
	{
		FVector eyeLocation = controlledPawn->GetActorLocation() + FVector(0, 0, 60.0f);
		FVector eyeDirection = controlledPawn->GetActorForwardVector();

		FColor sightColor = bIsSeeingTarget ? FColor::Red : FColor::Green;

		DrawDebugCone(
			GetWorld(),
			eyeLocation,
			eyeDirection,
			SightConfig->SightRadius,
			FMath::DegreesToRadians(SightConfig->PeripheralVisionAngleDegrees),
			FMath::DegreesToRadians(0.5f),
			24, 
			sightColor,
			false, -1.0f, 0, 2.0f
		);
	}
#endif
}

void AOecAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (AOecPlayerCharacter* player = Cast<AOecPlayerCharacter>(Actor))
	{
		bIsSeeingTarget = Stimulus.WasSuccessfullySensed();

		if (UBlackboardComponent* blackboardComp = GetBlackboardComponent())
		{
			if (bIsSeeingTarget)
			{
				blackboardComp->SetValueAsObject(TEXT("TargetActor"), player);
			}
			else
			{
				blackboardComp->ClearValue(TEXT("TargetActor"));
			}
		}
	}
}

void AOecAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}
