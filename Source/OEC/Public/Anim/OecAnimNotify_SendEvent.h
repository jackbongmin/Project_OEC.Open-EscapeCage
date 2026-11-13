// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "OecAnimNotify_SendEvent.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecAnimNotify_SendEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UOecAnimNotify_SendEvent();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* InMeshComp, UAnimSequenceBase* InAnimation, const FAnimNotifyEventReference& InEventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|GAS")
	FGameplayTag EventTag;
};
