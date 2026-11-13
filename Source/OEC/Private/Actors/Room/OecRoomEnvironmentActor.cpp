// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/OecRoomEnvironmentActor.h"
#include "Choice/OecChoiceSubsystem.h"
#include "GAS/OecGameplayTags.h"
#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/LightComponent.h"
#include "Engine/Light.h"

AOecRoomEnvironmentActor::AOecRoomEnvironmentActor()
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

void AOecRoomEnvironmentActor::BeginPlay()
{
	Super::BeginPlay();

	if (ReactEffectTags.IsEmpty())
	{
		ReactEffectTags.AddTag(OecGameplayTags::Effect_Env);
	}

	UOecChoiceSubsystem* choiceSubsystem = GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>();
	if (!choiceSubsystem) return;

	choiceSubsystem->OnAnswerModifierChanged.AddUniqueDynamic(this, &AOecRoomEnvironmentActor::OnAnswerModifierChanged);

	// 이미 걸려 있는 환경 효과 반영 (레벨 스트리밍으로 늦게 로드된 경우)
	for (const FOecAnswerModifier& modifier : choiceSubsystem->GetAnswerModifiers())
	{
		OnAnswerModifierChanged(modifier, true);
	}
}

void AOecRoomEnvironmentActor::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	if (UOecChoiceSubsystem* choiceSubsystem = GetGameInstance()->GetSubsystem<UOecChoiceSubsystem>())
	{
		choiceSubsystem->OnAnswerModifierChanged.RemoveDynamic(this, &AOecRoomEnvironmentActor::OnAnswerModifierChanged);
	}

	Super::EndPlay(InEndPlayReason);
}

void AOecRoomEnvironmentActor::OnAnswerModifierChanged(const FOecAnswerModifier& InModifier, bool bInActive)
{
	if (!InModifier.EffectTag.MatchesAny(ReactEffectTags)) return;

	if (bInActive)
	{
		ActiveEffectTags.AddTag(InModifier.EffectTag);
	}
	else
	{
		ActiveEffectTags.RemoveTag(InModifier.EffectTag);
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] 환경 효과 %s: %s"), *GetName(), bInActive ? TEXT("시작") : TEXT("종료"), *InModifier.EffectTag.ToString());

	if (InModifier.EffectTag == OecGameplayTags::Effect_Env_Darken)
	{
		HandleDarken(bInActive);
	}

	BP_OnEnvironmentEffectChanged(InModifier.EffectTag, bInActive, InModifier.Value);
}

void AOecRoomEnvironmentActor::HandleDarken(bool bInActive)
{
	for (ALight* light : DarkenLights)
	{
		if (!IsValid(light)) continue;

		ULightComponent* lightComponent = light->GetLightComponent();
		if (!lightComponent) continue;

		if (lightComponent->Mobility == EComponentMobility::Static)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] 조명 [%s]의 모빌리티가 스태틱이라 밝기를 바꿀 수 없습니다"), *GetName(), *light->GetName());
			continue;
		}

		if (bInActive)
		{
			// 이미 어두운 상태에서 다시 걸려도 원래 밝기는 처음 값으로 유지
			const float* originalIntensity = OriginalIntensities.Find(lightComponent);
			const float baseIntensity = originalIntensity ? *originalIntensity : lightComponent->Intensity;

			OriginalIntensities.Add(lightComponent, baseIntensity);
			lightComponent->SetIntensity(baseIntensity * DarkenIntensityScale);
		}
		else if (const float* originalIntensity = OriginalIntensities.Find(lightComponent))
		{
			lightComponent->SetIntensity(*originalIntensity);
		}
	}

	if (!bInActive)
	{
		OriginalIntensities.Empty();
	}
}
