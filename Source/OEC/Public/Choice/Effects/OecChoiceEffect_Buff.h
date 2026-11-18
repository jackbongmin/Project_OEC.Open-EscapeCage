// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_Buff.generated.h"

/**
 * Effect.Buff: 버프·디버프 GE를 답변 N번 동안 적용
 * - Param = 버프 ID (프로젝트 설정의 ChoiceBuffEffects), Value = 지속 답변 수 (0 이하면 설정의 기본값)
 * - 시간제가 아닌 답변 횟수 기준: 버튼을 누르지 않고 기다리는 것으로 디버프를 피할 수 없게 한다
 * - GE는 무한 지속(Infinite)으로 만들고, 지속 효과(AnswerModifier)가 끝날 때 이 실행기가 제거한다
 * - 같은 버프를 다시 얻으면 GE를 겹쳐 걸지 않고 남은 답변 수만 갱신한다
 */
UCLASS()
class OEC_API UOecChoiceEffect_Buff : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Buff();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
	virtual bool IsHarmful(const FOecChoiceEffect& InEffect) const override;
	virtual void OnModifierExpired(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const override;

private:
	TSubclassOf<UGameplayEffect> FindBuffEffectClass(FName InBuffID) const;

	int32 GetDuration(const FOecChoiceEffect& InEffect) const;
};
