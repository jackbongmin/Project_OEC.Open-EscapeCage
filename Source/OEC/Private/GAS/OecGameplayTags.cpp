// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/OecGameplayTags.h"

namespace OecGameplayTags
{
	// 캐릭터 구분
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_Player, "Character.Player", "플레이어 캐릭터");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_Monster, "Character.Monster", "몬스터 캐릭터");

	// SetByCaller 데이터
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Data.Damage", "데미지 GE에 넘기는 피해량");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Heal, "Data.Heal", "회복 GE에 넘기는 회복량");

	// 게임플레이 이벤트
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Monster_Attack, "Event.Monster.Attack", "몬스터 공격 판정 애님 노티파이");

	// 선택지 효과
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_None, "Effect.None", "아무 일 없음 / 무의미 질문");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Stack, "Effect.Stack", "스택 증감 (페이즈 1: 전구, 페이즈 2: 열쇠). Value = 증감량");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Stat_Health, "Effect.Stat.Health", "체력 증감. Value = 증감량 (SetByCaller)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Stat_Sanity, "Effect.Stat.Sanity", "정신력 증감. Value = 증감량 (SetByCaller)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Death, "Effect.Death", "즉시 사망");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Buff, "Effect.Buff", "버프 GE 적용. Param = 버프 ID, Value = 레벨");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Spawn_Monster, "Effect.Spawn.Monster", "몬스터 소환. Param = 몬스터 코드(Random 가능), Value = 마리 수");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Give_Item, "Effect.Give.Item", "아이템 지급. Param = 아이템 코드, Value = 수량");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Passive_AnswerStackChance, "Effect.Passive.AnswerStackChance", "이후 답변마다 확률로 스택 +1. Value = 확률(0.01 = 1%), 중첩");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Env, "Effect.Env", "환경 효과 (부모 태그)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_Env_Darken, "Effect.Env.Darken", "방 어두워짐. Value = 지속 답변 수 (0 이하 = 영구)");

	// 탄약 종류
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ammo_Pistol, "Ammo.Pistol", "권총 탄약");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ammo_Rifle, "Ammo.Rifle", "라이플 탄약");
}
