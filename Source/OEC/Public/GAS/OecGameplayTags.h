// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * OEC 네이티브 게임플레이 태그
 *
 * - 프로젝트에서 C++이 사용하는 모든 태그는 여기에 선언한다.
 * - 문자열로 태그를 찾지 않는다. (오타가 컴파일 에러로 잡힘)
 * - 엔진 시작 시 자동 등록되므로 DefaultGameplayTags.ini에 따로 추가하지 않는다.
 * - 새 태그는 분류(네임스페이스 안 주석 블록)에 맞춰 추가하고, 선언과 정의(.cpp)를 함께 수정한다.
 */
namespace OecGameplayTags
{
	/* ==========================================
		캐릭터 구분
	========================================== */
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Player);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Monster);

	/* ==========================================
		SetByCaller 데이터
	========================================== */
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Heal);

	/* ==========================================
		게임플레이 이벤트
	========================================== */
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Monster_Attack);

	/* ==========================================
		선택지 효과 (질문 버튼 결과)
		- 효과 하나 = 태그 하나 = 실행 클래스(UOecChoiceEffectExecutor) 하나
		- 스탯 효과 태그는 GE의 SetByCaller 태그로도 사용
	========================================== */
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_None);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Stack);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Stat_Health);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Stat_Sanity);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Death);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Buff);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Spawn_Monster);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Give_Item);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Passive_AnswerStackChance);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Env);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Env_Darken);

	/* ==========================================
		탄약 종류
	========================================== */
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ammo_Pistol);
	OEC_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ammo_Rifle);
}
