// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Data/OecChoiceTypes.h"
#include "OecRoomEnvironmentActor.generated.h"

class UBillboardComponent;
class ALight;
class ULightComponent;

/**
 * 방 환경 반응 액터 (조명, 포스트 프로세스 등)
 *
 * - 선택지의 환경 효과(Effect.Env.*)가 걸리거나 끝날 때 반응한다.
 * - 방 어두워짐(Effect.Env.Darken): DarkenLights에 지정한 조명 밝기를 DarkenIntensityScale 배율로 낮추고, 끝나면 원래 밝기로 되돌린다.
 * - 추가 연출(소리, 포스트 프로세스 등)이 필요하면 BP_OnEnvironmentEffectChanged에서 블루프린트로 처리한다.
 * - 방마다 하나씩 배치하고, 반응할 태그를 ReactEffectTags로 고른다.
 */
UCLASS()
class OEC_API AOecRoomEnvironmentActor : public AActor
{
	GENERATED_BODY()

public:
	AOecRoomEnvironmentActor();

	// 환경 효과 시작(bInActive = true) / 종료(false). Value는 효과 세기
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Environment")
	void BP_OnEnvironmentEffectChanged(FGameplayTag InEffectTag, bool bInActive, float InValue);

	UFUNCTION(BlueprintPure, Category = "OEC|Environment")
	bool IsEffectActive(FGameplayTag InEffectTag) const { return ActiveEffectTags.HasTagExact(InEffectTag); }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

	UFUNCTION()
	void OnAnswerModifierChanged(const FOecAnswerModifier& InModifier, bool bInActive);

	// 방 어두워짐 적용 / 해제
	void HandleDarken(bool bInActive);

protected:
	// 반응할 효과 태그 (비워두면 Effect.Env 하위 전체)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Environment", meta = (Categories = "Effect.Env"))
	FGameplayTagContainer ReactEffectTags;

	// 어두워질 때 밝기를 낮출 조명들 (맵에서 스포이드로 지정. 모빌리티가 스태틱이면 동작하지 않음)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "OEC|Environment|Darken")
	TArray<TObjectPtr<ALight>> DarkenLights;

	// 어두워졌을 때 밝기 배율 (0 = 완전히 꺼짐, 0.2 = 원래 밝기의 20%)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Environment|Darken", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DarkenIntensityScale = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<USceneComponent> SceneRoot;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "OEC|Components")
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif

private:
	FGameplayTagContainer ActiveEffectTags;

	// 어두워지기 전 원래 밝기 (되돌릴 때 사용)
	TMap<TWeakObjectPtr<ULightComponent>, float> OriginalIntensities;
};
