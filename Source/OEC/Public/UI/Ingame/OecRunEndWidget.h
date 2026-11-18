// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "Data/OecEnumType.h"
#include "OecRunEndWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * 회차 종료 화면 (사망, 붕괴, 탈출)
 *
 * - 블루프린트 없이 C++ 클래스만 써도 기본 화면(반투명 배경 + 제목 + 메시지 + 버튼)이 만들어진다.
 * - 디자인할 때는 이 클래스를 부모로 위젯 블루프린트를 만들고, 같은 이름의 위젯을 배치하면 그쪽을 쓴다.
 *   Txt_Title, Txt_Message, Btn_Restart, Btn_Quit (전부 선택. 없는 것은 무시)
 * - 사유별 문구는 디테일 패널의 TitleTexts / MessageTexts에서 바꾼다.
 */
UCLASS()
class OEC_API UOecRunEndWidget : public UOecBaseWidget
{
	GENERATED_BODY()

public:
	UOecRunEndWidget(const FObjectInitializer& InObjectInitializer);

	// 종료 사유와 회차에 맞게 문구 갱신
	UFUNCTION(BlueprintCallable, Category = "OEC|Run")
	void SetRunEndInfo(EOecRunEndReason InReason, int32 InRunCount);

protected:
	virtual void NativeOnInitialized() override;

	// 블루프린트 연출용 (페이드 인, 소리 등)
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Run")
	void BP_OnRunEndInfoSet(EOecRunEndReason InReason, int32 InRunCount);

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnQuitClicked();

private:
	// 위젯 블루프린트로 배치하지 않았을 때 쓰는 기본 화면
	void BuildDefaultLayout();

	UTextBlock* MakeText(const FString& InText, int32 InFontSize, const FLinearColor& InColor);

	UButton* MakeButton(const FString& InLabel);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_Title;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Txt_Message;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_Restart;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_Quit;

	// 종료 사유별 제목
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Run")
	TMap<EOecRunEndReason, FText> TitleTexts;

	// 종료 사유별 WARDEN 대사 ({0} = 끝난 회차)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OEC|Run")
	TMap<EOecRunEndReason, FText> MessageTexts;
};
