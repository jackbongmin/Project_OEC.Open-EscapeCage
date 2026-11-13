// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/OecAnimNotify_SendEvent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UOecAnimNotify_SendEvent::UOecAnimNotify_SendEvent()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 128, 0, 255);
#endif
}

FString UOecAnimNotify_SendEvent::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		return EventTag.ToString();
	}
	return Super::GetNotifyName_Implementation();
}

void UOecAnimNotify_SendEvent::Notify(USkeletalMeshComponent* InMeshComp, UAnimSequenceBase* InAnimation, const FAnimNotifyEventReference& InEventReference)
{
	Super::Notify(InMeshComp, InAnimation, InEventReference);

	if (InMeshComp && InMeshComp->GetOwner())
	{
		AActor* ownerActor = InMeshComp->GetOwner();

		FGameplayEventData payloadData;
		payloadData.Instigator = ownerActor;
		payloadData.Target = ownerActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ownerActor, EventTag, payloadData);
	}
}
