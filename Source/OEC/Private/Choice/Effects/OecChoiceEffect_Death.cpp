// Fill out your copyright notice in the Description page of Project Settings.


#include "Choice/Effects/OecChoiceEffect_Death.h"
#include "Settings/OecGameDataSettings.h"
#include "GAS/OecGameplayTags.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

UOecChoiceEffect_Death::UOecChoiceEffect_Death()
{
	EffectTag = OecGameplayTags::Effect_Death;
}

void UOecChoiceEffect_Death::Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const
{
	UAbilitySystemComponent* asc = GetInstigatorASC(InContext);
	if (!asc) return;

	TSubclassOf<UGameplayEffect> healthEffectClass = GetDefault<UOecGameDataSettings>()->ChoiceHealthEffect.LoadSynchronous();
	if (!healthEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Effect.Death] 프로젝트 설정에 ChoiceHealthEffect가 등록되지 않았습니다"));
		return;
	}

	// 체력 GE를 재사용해 현재 체력만큼 감소 (SetByCaller 태그는 Effect.Stat.Health)
	const float currentHealth = asc->GetNumericAttribute(UOecAttributeSetBase::GetHealthAttribute());

	FGameplayEffectContextHandle contextHandle = asc->MakeEffectContext();
	contextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle specHandle = asc->MakeOutgoingSpec(healthEffectClass, 1.0f, contextHandle);
	if (!specHandle.IsValid()) return;

	specHandle.Data.Get()->SetSetByCallerMagnitude(OecGameplayTags::Effect_Stat_Health, -currentHealth);
	asc->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());

	UE_LOG(LogTemp, Warning, TEXT("즉사 효과 발동"));
}

FString UOecChoiceEffect_Death::BuildDescription(const FOecChoiceEffect& InEffect) const
{
	return TEXT("즉시 사망");
}
