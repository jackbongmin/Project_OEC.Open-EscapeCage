// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OecBaseWidget.h"
#include "Kismet/GameplayStatics.h"

void UOecBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 초기화 관련 공용 로직이 필요하면 여기에 작성
}

void UOecBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 화면에 뜰 때마다 실행할 공용 로직이 필요하면 여기에 작성
}
void UOecBaseWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UOecBaseWidget::HideWidget()
{
	// HitTestInvisible로 숨기면 클릭은 무시하면서 안 보이게 됨 (성능 유리)
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOecBaseWidget::PlayUISound(USoundBase* InSound)
{
	if (InSound)
	{
		UGameplayStatics::PlaySound2D(this, InSound);
	}
}

