// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecCrosshairWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"

void UOecCrosshairWidget::OnWeaponStateChanged(bool bIsArmed)
{
	if (DotImage && CrossCanvas)
	{
		if (bIsArmed)
		{
			// 무기 장착: 점 숨기고, 십자선 켜기
			DotImage->SetVisibility(ESlateVisibility::Hidden);
			CrossCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible); 
		}
		else
		{
			// 맨손: 점 켜고, 십자선 숨기기
			DotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			CrossCanvas->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UOecCrosshairWidget::OnFireWeapon()
{
	if (!bIsCurrentlyAiming)
	{
		if (Anim_Fire)
		{
			PlayAnimation(Anim_Fire);
		}
	}
	else
	{
		
	}
}

void UOecCrosshairWidget::OnAimStateChanged(bool bIsAiming)
{
	bIsCurrentlyAiming = bIsAiming;

	if (DotImage && CrossCanvas)
	{
		if (bIsAiming)
		{
			// 조준 중: 점 나타나고, 십자선 모이는 애니메이션
			DotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (Anim_Aim) PlayAnimation(Anim_Aim, 0.0f, 1, EUMGSequencePlayMode::Forward);
		}
		else
		{
			// 지향 사격: 점 숨기고, 십자선 벌어지는 애니메이션
			DotImage->SetVisibility(ESlateVisibility::Hidden);
			if (Anim_Aim) PlayAnimation(Anim_Aim, 0.0f, 1, EUMGSequencePlayMode::Reverse);
		}
	}
}
