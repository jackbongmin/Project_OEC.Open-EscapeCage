// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/OecGameplayAbility.h"
#include "OecGA_UseItem.generated.h"

struct FItemStaticData;

/**
 * 소모품 사용 이벤트에 실어 보내는 정보 (어떤 아이템을 썼는지)
 */
UCLASS()
class OEC_API UOecItemUsePayload : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName ItemCode;
};

/**
 * 소모품 사용 어빌리티 (모든 소모품 공용)
 *
 * - Event.Item.Use 게임플레이 이벤트로 발동한다. 어떤 아이템인지는 이벤트의 OptionalObject(UOecItemUsePayload)로 받는다.
 * - 효과는 아이템 데이터에서 읽는다: 체력 회복량, 정신력 회복량, UseEffects(선택지 효과 구조)
 *   → 새 소모품은 데이터 테이블 행만 추가하면 되고, 어빌리티를 새로 만들 필요가 없다.
 * - 스탯 변화는 선택지 효과 실행기를 거쳐 GE(SetByCaller)로 적용된다.
 */
UCLASS()
class OEC_API UOecGA_UseItem : public UOecGameplayAbility
{
	GENERATED_BODY()

public:
	UOecGA_UseItem();

protected:
	// 효과를 지금 쓸 수 없는 아이템이면 이벤트에 반응하지 않는다 → 아이템이 소모되지 않음 (예: 스토리 질문에 질문 변경권)
	virtual bool ShouldAbilityRespondToEvent(const FGameplayAbilityActorInfo* InActorInfo, const FGameplayEventData* InPayload) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData) override;

private:
	const FItemStaticData* FindUsedItemData(const FGameplayAbilityActorInfo* InActorInfo, const FGameplayEventData* InPayload) const;

	void ApplyItemEffects(const FItemStaticData& InItemData, AActor* InAvatar) const;
};
