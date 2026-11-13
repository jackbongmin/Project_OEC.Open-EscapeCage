// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributeset/OecAttributeSetBase.h"
#include "GameplayEffectExtension.h"

UOecAttributeSetBase::UOecAttributeSetBase()
{
    InitHealth(100.f);
    InitMaxHealth(100.f);
    InitAttackPower(10.f);
    InitSpeed(600.f);
}

void UOecAttributeSetBase::PreAttributeChange(const FGameplayAttribute& InAttribute, float& InNewValue)
{
}

void UOecAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 최대 체력 기준으로 클램프 (몬스터마다 MaxHealth가 다름)
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		if (GEngine)
		{
			FString LogMsg = FString::Printf(TEXT("💊 꿀꺽! 현재 체력: %f"), GetHealth());
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, LogMsg);
		}

		UE_LOG(LogTemp, Warning, TEXT("💊 꿀꺽! 현재 체력: %f"), GetHealth());
	}
}
