// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Buff.h"
#include "Settings/OecGameDataSettings.h"
#include "GAS/OecGameplayTags.h"
#include "GameplayEffect.h"

UOecChoiceEffect_Buff::UOecChoiceEffect_Buff()
{
	EffectTag = OecGameplayTags::Effect_Buff;
}

void UOecChoiceEffect_Buff::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	const TSoftClassPtr<UGameplayEffect>* buffEffect = GetDefault<UOecGameDataSettings>()->ChoiceBuffEffects.Find(InEffect.Param);
	TSubclassOf<UGameplayEffect> effectClass = buffEffect ? buffEffect->LoadSynchronous() : nullptr;

	if (!effectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Effect.Buff] 버프 ID [%s]가 프로젝트 설정에 등록되지 않았습니다"), *InEffect.Param.ToString());
		return;
	}

	const float level = FMath::Max(InEffect.Value, 1.0f);
	if (ApplyEffectToInstigator(InContext, effectClass, InEffect.Value, level))
	{
		UE_LOG(LogTemp, Log, TEXT("버프 [%s] 적용 (레벨 %g)"), *InEffect.Param.ToString(), level);
	}
}

FString UOecChoiceEffect_Buff::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return FString::Printf(TEXT("%s 버프 획득"), *InEffect.Param.ToString());
}
