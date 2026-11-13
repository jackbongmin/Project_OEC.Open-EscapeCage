// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Data/OecChoiceTypes.h"
#include "OecChoiceEffectExecutor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

/**
 * 선택지 효과 실행기 (베이스)
 *
 * - 효과 태그 1개 = 실행 클래스 1개
 * - UOecChoiceSubsystem이 시작할 때 이 클래스의 모든 하위 클래스를 찾아 EffectTag로 자동 등록한다.
 *   → 새 효과는 하위 클래스 하나만 추가하면 되고, 기존 코드는 수정하지 않는다.
 * - 실행기는 상태를 갖지 않는다. 지속되는 상태는 UOecChoiceSubsystem의 AnswerModifier로 관리한다.
 */
UCLASS(Abstract)
class OEC_API UOecChoiceEffectExecutor : public UObject
{
	GENERATED_BODY()

public:
	FGameplayTag GetEffectTag() const { return EffectTag; }

	// 효과 실행
	virtual void Execute(const FOecChoiceEffectContext& InContext, const FOecChoiceEffect& InEffect) const PURE_VIRTUAL(UOecChoiceEffectExecutor::Execute, );

	// 버튼에 표시할 문구 (예: "체력 10 회복")
	virtual FString BuildDescription(const FOecChoiceEffect& InEffect) const;

	// 문구 자동 생성 시 "아무 일 없음"으로 취급할지 (확률 문구에서 생략됨)
	virtual bool IsEmptyEffect() const { return false; }

	// 지속 효과: 이후 답변마다 호출
	virtual void OnAnswerTick(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const {}

	// 지속 효과가 끝났을 때 호출
	virtual void OnModifierExpired(const FOecChoiceEffectContext& InContext, const FOecAnswerModifier& InModifier) const {}

protected:
	UAbilitySystemComponent* GetInstigatorASC(const FOecChoiceEffectContext& InContext) const;

	UWorld* GetContextWorld(const FOecChoiceEffectContext& InContext) const;

	// 실행기를 소유한 게임 인스턴스 (문구 생성처럼 컨텍스트가 없을 때 데이터 조회용)
	UGameInstance* GetOwningGameInstance() const;

	// GE에 SetByCaller(EffectTag) 값을 넣어 버튼을 누른 플레이어에게 적용
	bool ApplyEffectToInstigator(const FOecChoiceEffectContext& InContext, TSubclassOf<UGameplayEffect> InEffectClass, float InSetByCallerValue, float InLevel = 1.0f) const;

	// 하위 클래스 생성자에서 지정
	FGameplayTag EffectTag;
};
