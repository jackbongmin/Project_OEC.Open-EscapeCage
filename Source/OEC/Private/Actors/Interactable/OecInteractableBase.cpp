// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/OecInteractableBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/OecInteractRangeComponent.h"
#include "Settings/OecGameDataSettings.h"
#include "Blueprint/UserWidget.h"

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

    InteractRange = CreateDefaultSubobject<UOecInteractRangeComponent>(TEXT("InteractRange"));
    InteractRange->SetupAttachment(RootComponent);
}

void AOecInteractableBase::BeginPlay()
{
    Super::BeginPlay();

    // 블루프린트에서 E 위젯 클래스를 안 넣었으면 프로젝트 설정의 기본 위젯 사용
    if (InteractPromptWidget && !InteractPromptWidget->GetWidgetClass())
    {
        const UOecGameDataSettings* settings = GetDefault<UOecGameDataSettings>();
        if (UClass* widgetClass = settings ? settings->DefaultInteractPromptWidget.LoadSynchronous() : nullptr)
        {
            InteractPromptWidget->SetWidgetClass(widgetClass);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] E 표시 위젯이 없습니다. 프로젝트 설정 > OEC Game Data Settings > Default Interact Prompt Widget을 지정하세요"), *GetName());
        }
    }
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

bool AOecInteractableBase::CanInteract_Implementation(AActor* InInteractor) const
{
    return bInteractionEnabled;
}

void AOecInteractableBase::SetInteractFocused_Implementation(bool bInFocused)
{
    SetInteractPromptVisible(bInFocused);
}

FVector AOecInteractableBase::GetInteractLocation_Implementation() const
{
    // 메시 중심 (피벗이 바닥이나 모서리에 있어도 시선 판정이 자연스럽게)
    return MeshComponent ? MeshComponent->Bounds.Origin : GetActorLocation();
}

void AOecInteractableBase::SetInteractionEnabled(bool bInEnabled)
{
    bInteractionEnabled = bInEnabled;

    if (!bInEnabled)
    {
        SetInteractPromptVisible(false);
    }
}
