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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Data")
    TMap<EQuestionType, float> QuestionTypeWeights;

public:	
	virtual void Tick(float DeltaTime) override;

};
