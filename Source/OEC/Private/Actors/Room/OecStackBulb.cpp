// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/OecStackBulb.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInterface.h"

AOecStackBulb::AOecStackBulb()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BulbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulbMesh"));
	BulbMesh->SetupAttachment(RootComponent);

	BulbLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BulbLight"));
	BulbLight->SetupAttachment(RootComponent);
	BulbLight->SetMobility(EComponentMobility::Movable);
	// 전구 5개가 동시에 켜지므로 그림자는 기본으로 끔 (필요하면 인스턴스에서 켬)
	BulbLight->SetCastShadows(false);
	BulbLight->SetAttenuationRadius(300.0f);

#if WITH_EDITORONLY_DATA
	OrderText = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("OrderText"));
	if (OrderText)
	{
		OrderText->SetupAttachment(RootComponent);
		OrderText->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
		OrderText->SetHorizontalAlignment(EHTA_Center);
		OrderText->SetWorldSize(24.0f);
		OrderText->SetHiddenInGame(true);
		OrderText->bIsEditorOnly = true;
	}
#endif
}

void AOecStackBulb::OnConstruction(const FTransform& InTransform)
{
	Super::OnConstruction(InTransform);

#if WITH_EDITORONLY_DATA
	if (OrderText)
	{
		OrderText->SetText(StackOrder > 0 ? FText::AsNumber(StackOrder) : FText::FromString(TEXT("순번 미지정")));
		OrderText->SetTextRenderColor(StackOrder > 0 ? FColor::White : FColor::Red);
	}
#endif

	// 에디터 미리보기용. 게임에서는 BeginPlay에서 다시 꺼진 상태로 맞춘다
	bIsLit = bPreviewLitInEditor;
	ApplyLitVisual();
}

void AOecStackBulb::BeginPlay()
{
	Super::BeginPlay();

	if (!UnlitMaterial || !LitMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 꺼짐/켜짐 머티리얼이 지정되지 않았습니다. 라이트만 바뀝니다"), *GetName());
	}

	bIsLit = false;
	ApplyLitVisual();
}

void AOecStackBulb::SetStackLit_Implementation(bool bInLit, bool bInPlayEffect)
{
	bIsLit = bInLit;
	ApplyLitVisual();

	BP_OnLitChanged(bInLit, bInPlayEffect);
}

void AOecStackBulb::ApplyLitVisual()
{
	if (BulbLight)
	{
		BulbLight->SetVisibility(bIsLit);
	}

	if (!BulbMesh) return;

	UMaterialInterface* material = bIsLit ? LitMaterial : UnlitMaterial;
	if (!material) return;

	if (MaterialSlotIndex < 0)
	{
		for (int32 i = 0; i < BulbMesh->GetNumMaterials(); ++i)
		{
			BulbMesh->SetMaterial(i, material);
		}
	}
	else if (MaterialSlotIndex < BulbMesh->GetNumMaterials())
	{
		BulbMesh->SetMaterial(MaterialSlotIndex, material);
	}
}
