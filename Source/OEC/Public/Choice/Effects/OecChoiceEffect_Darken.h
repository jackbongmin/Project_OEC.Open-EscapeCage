// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Choice/Effects/OecChoiceEffectExecutor.h"
#include "OecChoiceEffect_Darken.generated.h"

/**
 * Effect.Env.Darken: 방이 어두워짐. Value = 지속 답변 수 (0 이하 = 영구)
 * 실제 조명 처리는 AOecRoomEnvironmentActor가 OnAnswerModifierChanged를 구독해서 한다.
 */
UCLASS()
class OEC_API UOecChoiceEffect_Darken : public UOecChoiceEffectExecutor
{
	GENERATED_BODY()

public:
	UOecChoiceEffect_Darken();

	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const override;
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const override;
};
