// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OecStackIndicator.generated.h"

class UBillboardComponent;

/**
 * 페이즈 1 스택 표시기
 *
 * - UOecStageManagerSubsystem::OnStackChanged를 구독해, 1번 칸부터 스택 수만큼 켠다.
 * - 칸이 어떻게 켜지는지는 모른다. IOecStackDisplayInterface를 구현한 액터(전구, 모니터 등)면 무엇이든 된다.
 * - 기본(bAutoCollectDisplays): 맵의 표시 액터를 모아 각자의 StackOrder(1, 2, 3...) 순으로 정렬한다. 전구마다 순번만 지정하면 된다.
 * - 자동 수집을 끄면 StackDisplays 배열 순서를 그대로 쓴다.
 */
UCLASS()
class OEC_API AOecStackIndicator : public AActor
{
	GENERATED_BODY()

public:
	AOecStackIndicator();

	// 스택 표시가 바뀐 뒤 호출 (전부 켜졌을 때 연출 등)
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Stack")
	void BP_OnStackDisplayed(int32 InCurrentStack, int32 InMaxStack);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

	UFUNCTION()
	void OnStackChanged(int32 InCurrentStack, int32 InMaxStack);

	// 맵에서 표시 액터를 모아 StackOrder 순으로 정렬
	void CollectDisplaysByOrder();

	// 스택 수에 맞춰 칸을 켜고 끔. bInPlayEffect가 false면 연출 없이 상태만 맞춘다
	void ApplyStack(int32 InCurrentStack, int32 InMaxStack, bool bInPlayEffect);

protected:
	// 맵의 표시 액터를 StackOrder로 자동 수집 (끄면 아래 배열을 직접 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Stack")
	bool bAutoCollectDisplays = true;

	// 켜질 순서대로 지정 (자동 수집을 끈 경우만. IOecStackDisplayInterface 구현 액터만)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "OEC|Stack", meta = (EditCondition = "!bAutoCollectDisplays", MustImplement = "/Script/OEC.OecStackDisplayInterface"))
	TArray<TObjectPtr<AActor>> StackDisplays;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<USceneComponent> SceneRoot;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "OEC|Components")
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif

private:
	// 지금 켜져 있는 칸 수 (바뀐 칸에만 연출을 재생하기 위함)
	int32 DisplayedStack = 0;
};
