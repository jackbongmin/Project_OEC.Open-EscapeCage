// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Buff.h"
#include "Choice/OecChoiceSubsystem.h"
#include "Settings/OecGameDataSettings.h"
#include "GAS/OecGameplayTags.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"

UOecChoiceEffect_Buff::UOecChoiceEffect_Buff()
{
	EffectTag = OecGameplayTags::Effect_Buff;
}

TSubclassOf<UGameplayEffect> UOecChoiceEffect_Buff::FindBuffEffectClass(FName InBuffID) const
{
	const TSoftClassPtr<UGameplayEffect>* buffEffect = GetDefault<UOecGameDataSettings>()->ChoiceBuffEffects.Find(InBuffID);
	return buffEffect ? buffEffect->LoadSynchronous() : nullptr;
}

int32 UOecChoiceEffect_Buff::GetDuration(const FOecChoiceEffect& InEffect) const
{
	const int32 duration = FMath::RoundToInt(InEffect.Value);
	return duration > 0 ? duration : GetDefault<UOecGameDataSettings>()->DefaultBuffAnswerDuration;
}

void UOecChoiceEffect_Buff::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	if (!InContext.ChoiceSubsystem) return;

	TSubclassOf<UGameplayEffect> effectClass = FindBuffEffectClass(InEffect.Param);
	if (!effectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Effect.Buff] 버프 ID [%s]가 프로젝트 설정에 등록되지 않았습니다"), *InEffect.Param.ToString());
		return;
	}

	const int32 duration = GetDuration(InEffect);

	// 1. 이미 걸려 있으면 GE는 그대로 두고 남은 답변 수만 갱신
	if (InContext.ChoiceSubsystem->HasAnswerModifier(EffectTag, InEffect.Param))
	{
		InContext.ChoiceSubsystem->AddAnswerModifier(EffectTag, 1.0f, duration, false, InEffect.Param);
		return;
	}

	// 2. 새로 걸기: GE 적용 + 답변 횟수 카운트 시작
	if (ApplyEffectToInstigator(InContext, effectClass, 0.0f))
	{
		InContext.ChoiceSubsystem->AddAnswerModifier(EffectTag, 1.0f, duration, false, InEffect.Param);
		UE_LOG(LogTemp, Log, TEXT("버프 [%s] 적용 (답변 %d번 동안)"), *InEffect.Param.ToString(), duration);
	}
}

void UOecChoiceEffect_Buff::OnModifierExpired(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const
{
	UAbilitySystemComponent* asc = GetInstigatorASC(InContext);
	TSubclassOf<UGameplayEffect> effectClass = FindBuffEffectClass(InModifier.Param);
	if (!asc || !effectClass) return;

	asc->RemoveActiveGameplayEffectBySourceEffect(effectClass, nullptr);
	UE_LOG(LogTemp, Log, TEXT("버프 [%s] 종료"), *InModifier.Param.ToString());
}

FString UOecChoiceEffect_Buff::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	const FText* displayName = GetDefault<UOecGameDataSettings>()->ChoiceBuffDisplayNames.Find(InEffect.Param);
	const FString buffName = displayName ? displayName->ToString() : InEffect.Param.ToString();
	return FString::Printf(TEXT("%s (답변 %d번 동안)"), *buffName, GetDuration(InEffect));
}
