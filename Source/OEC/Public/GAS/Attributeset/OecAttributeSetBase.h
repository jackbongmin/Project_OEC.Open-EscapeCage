// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "OecAttributeSetBase.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class OEC_API UOecAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:
    UOecAttributeSetBase();

public:
    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UOecAttributeSetBase, Health)

        UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UOecAttributeSetBase, MaxHealth)

        UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UOecAttributeSetBase, AttackPower)

        UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData Speed;
    ATTRIBUTE_ACCESSORS(UOecAttributeSetBase, Speed)

        UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UOecAttributeSetBase, Defense)

        virtual void PreAttributeChange(const FGameplayAttribute& InAttribute, float& InNewValue) override;
protected:
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
