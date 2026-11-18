// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_Charge.generated.h"

/**
 * 충전식 효과 (베이스): 얻으면 횟수가 쌓이고, 다른 시스템이 필요할 때 1씩 소모한다
 * - Value = 충전 횟수 (0 이하면 1)
 * - 소모하는 쪽: 메모 = 단말기가 확률 질문을 띄울 때, 보험 = 나쁜 확률 결과가 나왔을 때, 소음 차단기 = 몬스터 소환 때
 * - 지속 효과(AnswerModifier)로 저장되므로 상태 아이콘 UI에도 그대로 나온다
 */
UCLASS(Abstract)
class OEC_API UOecChoiceEffect_Charge : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;

protected:
	// 하위 클래스 생성자에서 지정
	FString ChargeDescription;
};
