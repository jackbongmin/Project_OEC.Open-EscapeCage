// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_StatChange.generated.h"

/**
 * 스탯 증감 효과 (베이스)
 * 프로젝트 설정에 등록된 GE에 SetByCaller(EffectTag) 값으로 증감량을 넣어 적용한다.
 * 하위 클래스는 GE와 표시 이름만 지정한다.
 */
UCLASS(Abstract)
class OEC_API UOecChoiceEffect_StatChange : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;

protected:
	virtual TSoftClassPtr<UGameplayEffect> GetStatEffectClass() const PURE_VIRTUAL(UOecChoiceEffect_StatChange::GetStatEffectClass, return nullptr;);

	// 문구에 쓰는 스탯 이름 (예: "체력")
	FString StatDisplayName;
};
