// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"
#include "OecPlayerAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecPlayerAttributeSet : public UOecAttributeSetBase
{
	GENERATED_BODY()
	
public:
    UOecPlayerAttributeSet();

    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData Sanity;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, Sanity)

    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData MaxSanity;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, MaxSanity)


    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData AnswerCount;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, AnswerCount)

    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, MaxStamina)

    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData SprintSpeed;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, SprintSpeed)

    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData JumpForce;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, JumpForce)

    // 공격·재장전 속도 배율 (1 = 기본, 1.3 = 30% 빠름). 공격 속도 버프 GE가 곱연산으로 바꾼다
    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData AttackSpeed;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, AttackSpeed)

public:
    // 액터의 공격 속도 배율 (어트리뷰트가 없으면 1). 무기·어빌리티가 연사 간격과 몽타주 재생 속도에 곱한다
    static float GetAttackSpeedMultiplier(const AActor* InActor);
};
