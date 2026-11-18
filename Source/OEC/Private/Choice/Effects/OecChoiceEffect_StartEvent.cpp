// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_StartEvent.h"
#include "Actors/Room/Event/OecRoomEventBase.h"
#include "Actors/Room/OecChoiceSpawnPoint.h"
#include "Subsystems/OecGameDataSubsystem.h"
#include "Subsystems/OecSpawnPointSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "GAS/OecGameplayTags.h"

namespace
{
	const FName RandomEventID(TEXT("Random"));
}

UOecChoiceEffect_StartEvent::UOecChoiceEffect_StartEvent()
{
	EffectTag = OecGameplayTags::Effect_Event_Start;
}

const FOecRoomEventData* UOecChoiceEffect_StartEvent::FindEventData(FName InEventID) const
{
	const UGameInstance* gameInstance = GetOwningGameInstance();
	const UOecGameDataSubsystem* dataSubsystem = gameInstance ? gameInstance->GetSubsystem<UOecGameDataSubsystem>() : nullptr;
	return dataSubsystem ? dataSubsystem->GetRoomEventData(InEventID) : nullptr;
}

int32 UOecChoiceEffect_StartEvent::GetCurrentPhaseNumber() const
{
	const UGameInstance* gameInstance = GetOwningGameInstance();
	const UOecStageManagerSubsystem* stageManager = gameInstance ? gameInstance->GetSubsystem<UOecStageManagerSubsystem>() : nullptr;
	return (stageManager && stageManager->GetCurrentPhase() == EOecGamePhase::Phase2_Corridor) ? 2 : 1;
}

FName UOecChoiceEffect_StartEvent::ResolveEventID(FName InParam) const
{
	if (InParam != RandomEventID) return InParam;

	const UGameInstance* gameInstance = GetOwningGameInstance();
	const UOecGameDataSubsystem* dataSubsystem = gameInstance ? gameInstance->GetSubsystem<UOecGameDataSubsystem>() : nullptr;
	if (!dataSubsystem) return NAME_None;

	const int32 phaseNumber = GetCurrentPhaseNumber();
	TArray<FName> candidates;
	for (const FName& eventID : dataSubsystem->GetRoomEventIDs())
	{
		const FOecRoomEventData* eventData = dataSubsystem->GetRoomEventData(eventID);
		if (!eventData || !eventData->bAllowRandom) continue;
		if (eventData->AllowedPhases.Num() > 0 && !eventData->AllowedPhases.Contains(phaseNumber)) continue;

		candidates.Add(eventID);
	}

	return candidates.Num() > 0 ? candidates[FMath::RandRange(0, candidates.Num() - 1)] : NAME_None;
}

bool UOecChoiceEffect_StartEvent::CanExecute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	return ResolveEventID(InEffect.Param) != NAME_None;
}

FTransform UOecChoiceEffect_StartEvent::FindEventTransform(UWorld* InWorld, AActor* InPlayer) const
{
	if (UOecSpawnPointSubsystem* spawnPointSubsystem = InWorld->GetSubsystem<UOecSpawnPointSubsystem>())
	{
		if (AOecChoiceSpawnPoint* spawnPoint = spawnPointSubsystem->GetRandomSpawnPoint(EOecSpawnPointType::Event))
		{
			bool bFoundGround = false;
			return spawnPoint->GetRandomGroundTransform(bFoundGround);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Effect.Event.Start] 맵에 이벤트 기준점(OecChoiceSpawnPoint, 타입 Event)이 없어 플레이어 위치에 생성합니다"));
	return InPlayer ? FTransform(InPlayer->GetActorLocation()) : FTransform::Identity;
}

void UOecChoiceEffect_StartEvent::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	UWorld* world = GetContextWorld(InContext);
	if (!world) return;

	const FName eventID = ResolveEventID(InEffect.Param);
	const FOecRoomEventData* eventData = FindEventData(eventID);
	if (!eventData)
	{
		UE_LOG(LogTemp, Error, TEXT("[Effect.Event.Start] 이벤트 데이터가 없습니다: %s (DT_RoomEvent 확인)"), *InEffect.Param.ToString());
		return;
	}

	TSubclassOf<AOecRoomEventBase> eventClass = eventData->EventActorClass.LoadSynchronous();
	if (!eventClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Effect.Event.Start] [%s]의 이벤트 액터 클래스가 비어 있습니다"), *eventID.ToString());
		return;
	}

	AActor* player = InContext.Instigator;
	const FTransform spawnTransform = FindEventTransform(world, player);

	AOecRoomEventBase* roomEvent = world->SpawnActorDeferred<AOecRoomEventBase>(eventClass, spawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!roomEvent) return;

	roomEvent->FinishSpawning(spawnTransform);
	roomEvent->StartEvent(eventID, *eventData, player);
}

FString UOecChoiceEffect_StartEvent::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	if (InEffect.Param == RandomEventID) return TEXT("무작위 이벤트 발생");

	const FOecRoomEventData* eventData = FindEventData(InEffect.Param);
	if (!eventData) return FString::Printf(TEXT("이벤트 %s"), *InEffect.Param.ToString());

	return eventData->Description.IsEmpty() ? eventData->EventName : eventData->Description;
}

bool UOecChoiceEffect_StartEvent::IsHarmful(const FOecChoiceEffect& InEffect) const
{
	if (InEffect.Param == RandomEventID) return true;

	const FOecRoomEventData* eventData = FindEventData(InEffect.Param);
	return eventData && eventData->bHarmful;
}
