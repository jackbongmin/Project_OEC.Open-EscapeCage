// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_AnswerStat.generated.h"

/**
 * 답변마다 스탯 변화 패시브 (베이스)
 *
 * - Value = 답변 1회마다 변화량, Param = 지속 답변 수 (예: "10"). 비우면 DefaultDuration
 * - 같은 패시브를 다시 얻으면 변화량은 더해지고 남은 답변 수는 새 값으로 갱신된다
 * - 실제 스탯 변화는 StatEffectTag의 실행기(체력·정신력 GE)에 맡긴다
 */
UCLASS(Abstract)
class OEC_API UOecChoiceEffect_AnswerStat : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
	virtual void OnAnswerTick(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const override;

protected:
	int32 GetDuration(const FOecChoiceEffect& InEffect) const;

	// 하위 클래스 생성자에서 지정
	FGameplayTag StatEffectTag;

	FString StatName;

	int32 DefaultDuration = 10;
};
