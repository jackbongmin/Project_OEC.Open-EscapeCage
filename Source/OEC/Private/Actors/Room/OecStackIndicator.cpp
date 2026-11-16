// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/OecStackIndicator.h"
#include "Interfaces/OecStackDisplayInterface.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"

AOecStackIndicator::AOecStackIndicator()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

#if WITH_EDITORONLY_DATA
	BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	if (BillboardComponent)
	{
		BillboardComponent->SetupAttachment(RootComponent);
	}
#endif
}

void AOecStackIndicator::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoCollectDisplays)
	{
		CollectDisplaysByOrder();
	}
	else
	{
		// 인터페이스를 구현하지 않은 액터는 제외
		for (int32 i = StackDisplays.Num() - 1; i >= 0; --i)
		{
			AActor* display = StackDisplays[i];
			if (!IsValid(display) || !display->Implements<UOecStackDisplayInterface>())
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] %d번 칸이 비었거나 스택 표시 액터가 아닙니다. 제외합니다"), *GetName(), i);
				StackDisplays.RemoveAt(i);
			}
		}
	}

	UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>();
	if (!stageManager) return;

	stageManager->OnStackChanged.AddUniqueDynamic(this, &AOecStackIndicator::OnStackChanged);

	if (StackDisplays.Num() != stageManager->GetMaxStack())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 전구 개수(%d)가 최대 스택(%d)과 다릅니다. 프로젝트 설정의 Phase1MaxStack을 확인하세요"),
			*GetName(), StackDisplays.Num(), stageManager->GetMaxStack());
	}

	// 이미 쌓인 스택 반영 (레벨을 다시 열었거나 늦게 로드된 경우). 연출 없이 상태만 맞춘다
	DisplayedStack = 0;
	for (AActor* display : StackDisplays)
	{
		IOecStackDisplayInterface::Execute_SetStackLit(display, false, false);
	}
	ApplyStack(stageManager->GetCurrentStack(), stageManager->GetMaxStack(), false);
}

void AOecStackIndicator::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	if (UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>())
	{
		stageManager->OnStackChanged.RemoveDynamic(this, &AOecStackIndicator::OnStackChanged);
	}

	Super::EndPlay(InEndPlayReason);
}

void AOecStackIndicator::CollectDisplaysByOrder()
{
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithInterface(this, UOecStackDisplayInterface::StaticClass(), foundActors);

	StackDisplays.Reset();
	TSet<int32> usedOrders;

	for (AActor* display : foundActors)
	{
		const int32 order = IOecStackDisplayInterface::Execute_GetStackOrder(display);
		if (order <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] [%s]의 StackOrder가 지정되지 않아 제외합니다"), *GetName(), *display->GetName());
			continue;
		}

		if (usedOrders.Contains(order))
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] StackOrder %d번이 중복됩니다: [%s]"), *GetName(), order, *display->GetName());
		}
		usedOrders.Add(order);

		StackDisplays.Add(display);
	}

	StackDisplays.Sort([](const AActor& InA, const AActor& InB)
	{
		return IOecStackDisplayInterface::Execute_GetStackOrder(&InA) < IOecStackDisplayInterface::Execute_GetStackOrder(&InB);
	});

	// 1, 2, 3... 사이에 빈 번호가 있으면 경고 (예: 1, 2, 4)
	for (int32 i = 0; i < StackDisplays.Num(); ++i)
	{
		const int32 order = IOecStackDisplayInterface::Execute_GetStackOrder(StackDisplays[i]);
		if (order != i + 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] StackOrder가 1부터 빈틈없이 이어지지 않습니다 (%d번째 칸의 순번: %d)"), *GetName(), i + 1, order);
			break;
		}
	}
}

void AOecStackIndicator::OnStackChanged(int32 InCurrentStack, int32 InMaxStack)
{
	ApplyStack(InCurrentStack, InMaxStack, true);
}

void AOecStackIndicator::ApplyStack(int32 InCurrentStack, int32 InMaxStack, bool bInPlayEffect)
{
	const int32 newStack = FMath::Clamp(InCurrentStack, 0, StackDisplays.Num());

	// 상태가 바뀐 칸만 켜고 끔
	for (int32 i = 0; i < StackDisplays.Num(); ++i)
	{
		const bool bShouldLit = i < newStack;
		const bool bWasLit = i < DisplayedStack;
		if (bShouldLit == bWasLit) continue;

		if (AActor* display = StackDisplays[i])
		{
			IOecStackDisplayInterface::Execute_SetStackLit(display, bShouldLit, bInPlayEffect);
		}
	}

	DisplayedStack = newStack;

	UE_LOG(LogTemp, Log, TEXT("[%s] 스택 표시 %d / %d"), *GetName(), InCurrentStack, InMaxStack);

	BP_OnStackDisplayed(InCurrentStack, InMaxStack);
}
