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
};
