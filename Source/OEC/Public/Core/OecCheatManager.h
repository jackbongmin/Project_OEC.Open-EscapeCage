// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GameplayTagContainer.h"
#include "OecCheatManager.generated.h"

class UOecChoiceSubsystem;

/**
 * 개발용 치트 커맨드 (콘솔 ` 키로 입력, 쉬핑 빌드에서는 동작하지 않음)
 *
 * 예)
 *   OecAddStack 5
 *   OecSetPhase 2
 *   OecSpawnMonster Golem_N 2
 *   OecGiveItem Heal30
 *   OecRunEffect Effect.Passive.AnswerStackChance 0.5
 *   OecShowState
 */
UCLASS()
class OEC_API UOecCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	// 현재 페이즈의 스택 증감 (페이즈 1: 전구, 페이즈 2: 열쇠)
	UFUNCTION(Exec)
	void OecAddStack(int32 InAmount = 1);

	// 페이즈 변경 (1 또는 2)
	UFUNCTION(Exec)
	void OecSetPhase(int32 InPhase);

	UFUNCTION(Exec)
	void OecSetHealth(float InValue);

	UFUNCTION(Exec)
	void OecSetSanity(float InValue);

	// 몬스터 소환 (코드 생략 또는 Random = 무작위)
	UFUNCTION(Exec)
	void OecSpawnMonster(FName InMonsterCode = NAME_None, int32 InCount = 1);

	UFUNCTION(Exec)
	void OecGiveItem(FName InItemCode, int32 InCount = 1);

	// 아무 선택지 효과나 실행 (태그 이름, 값, 파라미터)
	UFUNCTION(Exec)
	void OecRunEffect(const FString& InEffectTagName, float InValue = 0.0f, FName InParam = NAME_None);

	// 페이즈, 스택, 정신력, 지속 효과 상태를 로그로 출력
	UFUNCTION(Exec)
	void OecShowState();

private:
	void RunEffect(const FGameplayTag& InEffectTag, float InValue, FName InParam);

	UOecChoiceSubsystem* GetChoiceSubsystem() const;

	APawn* GetPlayerPawn() const;
};
