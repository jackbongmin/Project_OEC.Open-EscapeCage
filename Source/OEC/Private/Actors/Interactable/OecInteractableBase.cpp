// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/OecInteractableBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AOecInteractableBase::AOecInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
    MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    InteractPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractPromptWidget"));
    InteractPromptWidget->SetupAttachment(RootComponent);

    InteractPromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractPromptWidget->SetDrawSize(FVector2D(200.0f, 50.0f));

    InteractPromptWidget->SetVisibility(false);

}

void AOecInteractableBase::SetInteractPromptVisible(bool bInVisible)
{
    if (InteractPromptWidget)
    {
		InteractPromptWidget->SetVisibility(bInVisible);
    }
}

void AOecInteractableBase::Interact_Implementation(AActor* InInteractor)
{
    UE_LOG(LogTemp, Log, TEXT("[%s] 기본 상호작용 호출됨!"), *GetName());
}

