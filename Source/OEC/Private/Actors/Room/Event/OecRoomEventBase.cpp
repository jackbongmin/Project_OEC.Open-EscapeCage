// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEventBase.h"
#include "Choice/OecChoiceSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "GAS/OecGameplayTags.h"
#include "Components/MeshComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

AOecRoomEventBase::AOecRoomEventBase()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
}

void AOecRoomEventBase::StartEvent(FName InEventID, const FOecRoomEventData& InEventData, AActor* InTargetPlayer)
{
	if (bStarted) return;

	bStarted = true;
	EventID = InEventID;
	EventData = InEventData;
	TargetPlayer = InTargetPlayer;

	// 1. 종료 조건 등록 (시간 / 답변 수). 둘 다 없으면 페이즈가 바뀔 때까지 유지
	if (EventData.DurationSeconds > 0.0f)
	{
		GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &AOecRoomEventBase::OnDurationElapsed, EventData.DurationSeconds, false);
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (EventData.DurationAnswers > 0)
	{
		RemainingAnswers = EventData.DurationAnswers;
		if (UOecChoiceSubsystem* choiceSubsystem = gameInstance->GetSubsystem<UOecChoiceSubsystem>())
		{
			choiceSubsystem->OnChoiceResolved.AddDynamic(this, &AOecRoomEventBase::OnChoiceResolved);
		}
	}

	if (UOecStageManagerSubsystem* stageManager = gameInstance->GetSubsystem<UOecStageManagerSubsystem>())
	{
		stageManager->OnPhaseChanged.AddDynamic(this, &AOecRoomEventBase::OnPhaseChanged);
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] 이벤트 시작: %s (%.0f초 / 답변 %d회)"), *GetName(), *EventData.EventName, EventData.DurationSeconds, EventData.DurationAnswers);

	// 2. 시작 효과 (예: 정전 = 방 어두워짐) → 하위 클래스 → 연출
	ExecuteEffects(EventData.StartEffects);
	OnEventStarted();
	BP_OnEventStarted();
}

void AOecRoomEventBase::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	// 서브시스템은 레벨이 바뀌어도 남으므로 구독을 직접 해제
	if (UGameInstance* gameInstance = GetGameInstance())
	{
		if (UOecChoiceSubsystem* choiceSubsystem = gameInstance->GetSubsystem<UOecChoiceSubsystem>())
		{
			choiceSubsystem->OnChoiceResolved.RemoveDynamic(this, &AOecRoomEventBase::OnChoiceResolved);
		}

		if (UOecStageManagerSubsystem* stageManager = gameInstance->GetSubsystem<UOecStageManagerSubsystem>())
		{
			stageManager->OnPhaseChanged.RemoveDynamic(this, &AOecRoomEventBase::OnPhaseChanged);
		}
	}

	Super::EndPlay(InEndPlayReason);
}

void AOecRoomEventBase::OnChoiceResolved(const FOecChoiceRecord& InRecord, const FOecChoiceOutcome& InOutcome)
{
	if (bFinished) return;

	if (--RemainingAnswers <= 0)
	{
		FinishByCondition();
	}
}

void AOecRoomEventBase::OnPhaseChanged()
{
	FinishByCondition();
}

void AOecRoomEventBase::OnDurationElapsed()
{
	FinishByCondition();
}

void AOecRoomEventBase::FinishByCondition()
{
	// 버티면 성공하는 이벤트: 피격 시 실패하는 규칙이 있으면 피격 여부도 본다
	const bool bSuccess = EventData.bSucceedOnTimeout && !(EventData.bFailOnPlayerHit && bPlayerHit);
	FinishEvent(bSuccess);
}

void AOecRoomEventBase::CompleteEvent(bool bInSuccess)
{
	FinishEvent(bInSuccess);
}

void AOecRoomEventBase::FinishEvent(bool bInSuccess)
{
	if (!bStarted || bFinished) return;

	bFinished = true;
	GetWorldTimerManager().ClearTimer(DurationTimerHandle);

	UE_LOG(LogTemp, Log, TEXT("[%s] 이벤트 종료: %s (%s)"), *GetName(), *EventData.EventName, bInSuccess ? TEXT("성공") : TEXT("실패"));

	ExecuteEffects(bInSuccess ? EventData.SuccessEffects : EventData.FailEffects);
	OnEventFinished(bInSuccess);
	BP_OnEventFinished(bInSuccess);

	if (DestroyDelay > 0.0f)
	{
		SetLifeSpan(DestroyDelay);
	}
	else
	{
		Destroy();
	}
}

void AOecRoomEventBase::ApplyDamageToPlayer(float InDamage)
{
	if (!bStarted || bFinished || InDamage <= 0.0f) return;

	bPlayerHit = true;

	FOecChoiceEffect damageEffect;
	damageEffect.EffectTag = OecGameplayTags::Effect_Stat_Health;
	damageEffect.Value = -InDamage;
	ExecuteEffects({ damageEffect });

	BP_OnPlayerHit(InDamage);
}

void AOecRoomEventBase::ExecuteEffects(const TArray<FOecChoiceEffect>& InEffects)
{
	if (InEffects.Num() == 0) return;

	UOecChoiceSubsystem* choiceSubsystem = GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>();
	if (!choiceSubsystem) return;

	for (const FOecChoiceEffect& effect : InEffects)
	{
		choiceSubsystem->ExecuteEffect(effect, TargetPlayer.Get());
	}
}

void AOecRoomEventBase::SetBasicShapeColor(UMeshComponent* InMesh, const FLinearColor& InColor)
{
	if (!InMesh || !InMesh->GetMaterial(0)) return;

	if (UMaterialInstanceDynamic* dynamicMaterial = InMesh->CreateDynamicMaterialInstance(0))
	{
		dynamicMaterial->SetVectorParameterValue(TEXT("Color"), InColor);
	}
}

FVector AOecRoomEventBase::FindFloorLocation(const FVector& InLocation) const
{
	FCollisionQueryParams queryParams(SCENE_QUERY_STAT(OecEventFloorTrace), false, this);
	FHitResult hitResult;

	const FVector traceStart = InLocation + FVector(0.0f, 0.0f, 200.0f);
	const FVector traceEnd = InLocation - FVector(0.0f, 0.0f, 3000.0f);
	if (GetWorld()->LineTraceSingleByObjectType(hitResult, traceStart, traceEnd, FCollisionObjectQueryParams(ECC_WorldStatic), queryParams))
	{
		return hitResult.ImpactPoint;
	}

	return InLocation;
}
