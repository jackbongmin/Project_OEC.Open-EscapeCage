// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OecHUD.h"
#include "UI/Ingame/OecQuickSlotPanelWidget.h"
#include "UI/Ingame/OecInGameWidget.h"
#include "Blueprint/UserWidget.h"
#include "UI/Ingame/OecRunEndWidget.h"
#include "Subsystems/OecRunSubsystem.h"
#include "Core/OecPlayerController.h"

void AOecHUD::BeginPlay()
{
    Super::BeginPlay();

	if (InGameWidgetClass)
	{
		InGameWidget = CreateWidget<UOecInGameWidget>(GetWorld(), InGameWidgetClass);
		if (InGameWidget)
		{
			InGameWidget->AddToViewport();
		}
	}

	if (UOecRunSubsystem* runSubsystem = GetGameInstance()->GetSubsystem<UOecRunSubsystem>())
	{
		runSubsystem->OnRunEnded.AddDynamic(this, &AOecHUD::OnRunEnded);
	}
}

void AOecHUD::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	// 회차 서브시스템은 레벨이 바뀌어도 남으므로 구독을 직접 해제
	if (UOecRunSubsystem* runSubsystem = GetGameInstance()->GetSubsystem<UOecRunSubsystem>())
	{
		runSubsystem->OnRunEnded.RemoveDynamic(this, &AOecHUD::OnRunEnded);
	}

	Super::EndPlay(InEndPlayReason);
}

void AOecHUD::OnRunEnded(EOecRunEndReason InReason, int32 InRunCount)
{
	// 치트 강제 재시작은 화면 없이 바로 넘어간다
	if (InReason == EOecRunEndReason::Restart) return;

	APlayerController* playerController = GetOwningPlayerController();
	if (!RunEndWidget)
	{
		TSubclassOf<UOecRunEndWidget> widgetClass = RunEndWidgetClass ? RunEndWidgetClass : TSubclassOf<UOecRunEndWidget>(UOecRunEndWidget::StaticClass());
		RunEndWidget = CreateWidget<UOecRunEndWidget>(playerController, widgetClass);
	}

	if (!RunEndWidget) return;

	RunEndWidget->SetRunEndInfo(InReason, InRunCount);
	if (!RunEndWidget->IsInViewport())
	{
		RunEndWidget->AddToViewport(100);
	}

	// 버튼을 누를 수 있게 마우스 표시, 이동·시점 입력 막기
	if (AOecPlayerController* oecController = Cast<AOecPlayerController>(playerController))
	{
		oecController->SetUIInputMode(true);
	}
}
