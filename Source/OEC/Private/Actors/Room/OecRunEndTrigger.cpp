// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/OecRunEndTrigger.h"
#include "Subsystems/OecRunSubsystem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AOecRunEndTrigger::AOecRunEndTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetHiddenInGame(true);
	RootComponent = TriggerBox;

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AOecRunEndTrigger::OnTriggerBeginOverlap);
}

void AOecRunEndTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult)
{
	// 플레이어가 조종하는 폰만 반응 (몬스터, 픽업 무시)
	const APawn* pawn = Cast<APawn>(InOtherActor);
	if (!pawn || !pawn->IsPlayerControlled()) return;

	UOecRunSubsystem* runSubsystem = GetGameInstance()->GetSubsystem<UOecRunSubsystem>();
	if (!runSubsystem || runSubsystem->IsRunEnding()) return;

	UE_LOG(LogTemp, Warning, TEXT("[%s] 회차 종료 트리거 (사유 %d)"), *GetName(), static_cast<int32>(EndReason));

	BP_OnTriggered(InOtherActor);
	runSubsystem->EndRun(EndReason);
}
