// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributeset/OecPlayerAttributeSet.h"
#include "GameplayEffectExtension.h"

UOecPlayerAttributeSet::UOecPlayerAttributeSet()
{
    // GDD 3.2: 정신력 80(노멀 상단)에서 시작
    InitSanity(80.f);
    InitMaxSanity(100.f);
    InitAnswerCount(0.f);

    InitStamina(100.f);
    InitMaxStamina(100.f);

    InitSprintSpeed(1500.f);
    InitJumpForce(600.f);
}

void UOecPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetSanityAttribute())
    {
        SetSanity(FMath::Clamp(GetSanity(), 0.0f, GetMaxSanity()));
        UE_LOG(LogTemp, Log, TEXT("현재 정신력: %f"), GetSanity());
    }
}
