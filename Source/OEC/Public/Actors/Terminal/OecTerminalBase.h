// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/OecEnumType.h"
#include "OecTerminalBase.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class AOecQuestionButton;
class UDataTable;

UCLASS()
class OEC_API AOecTerminalBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AOecTerminalBase();

	UFUNCTION(BlueprintCallable, Category = "OEC|Terminal")
	void InitializeTerminal();

    UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Terminal")
    void UpdateTerminalUI(const FString& InQuestionText);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    UWidgetComponent* ScreenWidget;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "OEC|Buttons")
    AOecQuestionButton* RedButton;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "OEC|Buttons")
    AOecQuestionButton* BlueButton;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Data")
    UDataTable* QuestionDataTable;

    // 타입별 출제 가중치 (페이즈 1, 그리고 아래 페이즈 2 가중치를 비워 뒀을 때)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Data")
    TMap<EQuestionType, float> QuestionTypeWeights;

    // 페이즈 2 가중치. 비워 두면 위 가중치를 그대로 쓴다
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Data")
    TMap<EQuestionType, float> Phase2QuestionTypeWeights;

    // 현재 페이즈에 맞는 가중치
    const TMap<EQuestionType, float>& GetCurrentTypeWeights() const;

public:	
	virtual void Tick(float DeltaTime) override;

};
