// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/OecAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Actors/Characters/Monster/OecMonsterBase.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"

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

	// 디버그 시야를 그리지 않으면 Tick이 필요 없음
	SetActorTickEnabled(bDrawSightDebug && bIsPerceptionActive);
}

void AOecAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	if (!bDrawSightDebug || !bIsPerceptionActive) return;

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
	// 죽은 몬스터는 감지 결과를 무시
	if (!bIsPerceptionActive) return;

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

	if (AOecMonsterBase* monster = Cast<AOecMonsterBase>(InPawn))
	{
		monster->OnMonsterActiveChanged.AddUniqueDynamic(this, &AOecAIController::OnMonsterActiveChanged);

		// 빙의 시점의 상태에 맞춤 (풀에 들어가 있던 몬스터에 빙의하는 경우)
		SetPerceptionActive(monster->IsMonsterActive());
	}
}

void AOecAIController::OnUnPossess()
{
	if (AOecMonsterBase* monster = Cast<AOecMonsterBase>(GetPawn()))
	{
		monster->OnMonsterActiveChanged.RemoveDynamic(this, &AOecAIController::OnMonsterActiveChanged);
	}

	Super::OnUnPossess();
}

void AOecAIController::OnMonsterActiveChanged(AOecMonsterBase* InMonster, bool bInActive)
{
	SetPerceptionActive(bInActive);
}

void AOecAIController::SetPerceptionActive(bool bInActive)
{
	if (bIsPerceptionActive == bInActive) return;

	bIsPerceptionActive = bInActive;

	// 1. 시야 감지 켜고 끄기. 끌 때는 기억한 대상도 지워서 부활 후 처음부터 다시 감지하게 함
	if (PerceptionComponent)
	{
		PerceptionComponent->SetSenseEnabled(UAISense_Sight::StaticClass(), bInActive);
		if (!bInActive)
		{
			PerceptionComponent->ForgetAll();
		}
	}

	// 2. 추적 대상 초기화
	bIsSeeingTarget = false;
	if (UBlackboardComponent* blackboardComp = GetBlackboardComponent())
	{
		blackboardComp->ClearValue(TEXT("TargetActor"));
	}

	// 3. BT 정지 / 재시작
	if (UBrainComponent* brain = GetBrainComponent())
	{
		if (bInActive)
		{
			brain->RestartLogic();
		}
		else
		{
			brain->StopLogic(TEXT("Monster Inactive"));
		}
	}

	if (!bInActive)
	{
		StopMovement();
	}

	// 4. 디버그 시야 표시 (Tick)
	SetActorTickEnabled(bDrawSightDebug && bInActive);

	UE_LOG(LogTemp, Log, TEXT("[%s] 시야 %s"), *GetName(), bInActive ? TEXT("켜짐") : TEXT("꺼짐"));
}
