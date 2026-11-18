// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecRunEndWidget.h"
#include "Subsystems/OecRunSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/KismetSystemLibrary.h"

UOecRunEndWidget::UOecRunEndWidget(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	TitleTexts.Add(EOecRunEndReason::Death, FText::FromString(TEXT("사망")));
	TitleTexts.Add(EOecRunEndReason::Collapse, FText::FromString(TEXT("붕괴")));
	TitleTexts.Add(EOecRunEndReason::Escape, FText::FromString(TEXT("탈출")));

	MessageTexts.Add(EOecRunEndReason::Death, FText::FromString(TEXT("\"{0}번째 기록을 종료합니다. 다시 시작합니다.\"")));
	MessageTexts.Add(EOecRunEndReason::Collapse, FText::FromString(TEXT("\"버티지 못했군요. 괜찮습니다. 처음부터 다시 하면 되니까요.\"")));
	MessageTexts.Add(EOecRunEndReason::Escape, FText::FromString(TEXT("\"축하합니다. 탈출입니다. ...정말 그렇게 생각하십니까?\"")));
}

void UOecRunEndWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 위젯 블루프린트 없이 C++ 클래스로 만들었으면 기본 화면을 직접 구성
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		BuildDefaultLayout();
	}

	if (Btn_Restart)
	{
		Btn_Restart->OnClicked.AddDynamic(this, &UOecRunEndWidget::OnRestartClicked);
	}

	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.AddDynamic(this, &UOecRunEndWidget::OnQuitClicked);
	}
}

void UOecRunEndWidget::BuildDefaultLayout()
{
	// 1. 화면 전체를 덮는 반투명 배경 (가운데 정렬)
	UBorder* background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Background"));
	background->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f));
	background->SetHorizontalAlignment(HAlign_Center);
	background->SetVerticalAlignment(VAlign_Center);
	WidgetTree->RootWidget = background;

	UVerticalBox* contentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
	background->SetContent(contentBox);

	// 2. 제목, 메시지, 버튼 순서로 세로 배치
	Txt_Title = MakeText(TEXT(""), 64, FLinearColor(0.8f, 0.05f, 0.05f));
	Txt_Message = MakeText(TEXT(""), 22, FLinearColor(0.85f, 0.85f, 0.85f));
	Btn_Restart = MakeButton(TEXT("다시 시작"));
	Btn_Quit = MakeButton(TEXT("게임 종료"));

	const TPair<UWidget*, float> rows[] = { { Txt_Title, 12.0f }, { Txt_Message, 40.0f }, { Btn_Restart, 12.0f }, { Btn_Quit, 0.0f } };
	for (const TPair<UWidget*, float>& row : rows)
	{
		if (UVerticalBoxSlot* boxSlot = contentBox->AddChildToVerticalBox(row.Key))
		{
			boxSlot->SetHorizontalAlignment(HAlign_Center);
			boxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, row.Value));
		}
	}
}

UTextBlock* UOecRunEndWidget::MakeText(const FString& InText, int32 InFontSize, const FLinearColor& InColor)
{
	UTextBlock* textBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	textBlock->SetText(FText::FromString(InText));
	textBlock->SetColorAndOpacity(FSlateColor(InColor));
	textBlock->SetJustification(ETextJustify::Center);
	textBlock->SetAutoWrapText(true);

	FSlateFontInfo fontInfo = textBlock->GetFont();
	fontInfo.Size = InFontSize;
	textBlock->SetFont(fontInfo);

	return textBlock;
}

UButton* UOecRunEndWidget::MakeButton(const FString& InLabel)
{
	UButton* button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	button->SetBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f));

	UTextBlock* label = MakeText(InLabel, 24, FLinearColor::White);
	label->SetAutoWrapText(false);
	button->AddChild(label);

	return button;
}

void UOecRunEndWidget::SetRunEndInfo(EOecRunEndReason InReason, int32 InRunCount)
{
	if (Txt_Title)
	{
		const FText* title = TitleTexts.Find(InReason);
		Txt_Title->SetText(title ? *title : FText::GetEmpty());
	}

	if (Txt_Message)
	{
		const FText* message = MessageTexts.Find(InReason);
		Txt_Message->SetText(message ? FText::Format(*message, FText::AsNumber(InRunCount)) : FText::GetEmpty());
	}

	BP_OnRunEndInfoSet(InReason, InRunCount);
}

void UOecRunEndWidget::OnRestartClicked()
{
	PlayUISound(DefaultClickSound);

	// 레벨이 다시 로드되면서 이 위젯과 입력 모드는 새 플레이어 컨트롤러가 정리한다
	if (UOecRunSubsystem* runSubsystem = GetGameInstance()->GetSubsystem<UOecRunSubsystem>())
	{
		SetIsEnabled(false);
		runSubsystem->RequestNextRun();
	}
}

void UOecRunEndWidget::OnQuitClicked()
{
	PlayUISound(DefaultClickSound);
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
