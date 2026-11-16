// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OecStackDisplayInterface.h"
#include "OecStackBulb.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class UMaterialInterface;
class UTextRenderComponent;

/**
 * 페이즈 1 스택 전구 한 개
 *
 * - 꺼짐/켜짐에 따라 메시 머티리얼을 UnlitMaterial / LitMaterial로 교체하고, 켜지면 포인트 라이트도 켠다.
 * - StackOrder로 몇 번째 스택에 켜질지 정한다 (1~최대 스택). 에디터 뷰포트에 번호가 표시된다.
 * - 라이트 색·밝기·감쇠 반경은 BulbLight 컴포넌트에서 직접 조절한다.
 * - 켜질 때 소리, 깜빡임 같은 연출은 BP_OnLitChanged에서 블루프린트로 처리한다.
 */
UCLASS()
class OEC_API AOecStackBulb : public AActor, public IOecStackDisplayInterface
{
	GENERATED_BODY()

public:
	AOecStackBulb();

	virtual void SetStackLit_Implementation(bool bInLit, bool bInPlayEffect) override;
	virtual int32 GetStackOrder_Implementation() const override { return StackOrder; }

	// 켜짐/꺼짐이 바뀐 뒤 호출. bInPlayEffect가 true일 때만 소리·연출을 재생하면 된다
	UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Stack")
	void BP_OnLitChanged(bool bInLit, bool bInPlayEffect);

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& InTransform) override;

	// 현재 bIsLit 상태를 메시·라이트에 반영
	void ApplyLitVisual();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UStaticMeshComponent> BulbMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
	TObjectPtr<UPointLightComponent> BulbLight;

	// 몇 번째 스택에 켜질지 (1부터). 맵에 배치한 전구마다 다르게 지정
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "OEC|Stack", meta = (ClampMin = "0"))
	int32 StackOrder = 0;

	// 꺼졌을 때 / 켜졌을 때 머티리얼
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Stack|Material")
	TObjectPtr<UMaterialInterface> UnlitMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Stack|Material")
	TObjectPtr<UMaterialInterface> LitMaterial;

	// 교체할 머티리얼 슬롯 번호 (-1 = 모든 슬롯)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Stack|Material", meta = (ClampMin = "-1"))
	int32 MaterialSlotIndex = 0;

	// 에디터에서 켜진 모습 미리보기 (게임 시작 시에는 꺼진 상태로 시작)
	UPROPERTY(EditAnywhere, Category = "OEC|Stack")
	bool bPreviewLitInEditor = false;

#if WITH_EDITORONLY_DATA
	// 에디터 뷰포트에 순번 표시 (게임에서는 안 보임)
	UPROPERTY(VisibleAnywhere, Category = "OEC|Components")
	TObjectPtr<UTextRenderComponent> OrderText;
#endif

private:
	bool bIsLit = false;

public:
	UFUNCTION(BlueprintPure, Category = "OEC|Stack")
	bool IsLit() const { return bIsLit; }
};
