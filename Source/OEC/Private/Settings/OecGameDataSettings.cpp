// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/OecGameDataSettings.h"

UOecGameDataSettings::UOecGameDataSettings()
{
	// 손해 질문은 2연속 금지 (QuestionGuideline.md 2.3)
	QuestionTypeStreakLimits.Add(EQuestionType::Penalty, 1);

	// 튜토리얼과 스토리는 회차마다 처음부터 다시 나온다
	RunScopedQuestionTypes.Add(EQuestionType::Tutorial);
	RunScopedQuestionTypes.Add(EQuestionType::Story);

	// 버프 표시 이름 (QuestionGuideline.md 3.4)
	ChoiceBuffDisplayNames.Add(TEXT("Buff_Haste"), FText::FromString(TEXT("이동 속도 증가")));
	ChoiceBuffDisplayNames.Add(TEXT("Buff_Jump"), FText::FromString(TEXT("점프력 증가")));
	ChoiceBuffDisplayNames.Add(TEXT("Buff_AttackSpeed"), FText::FromString(TEXT("공격 속도 증가")));
	ChoiceBuffDisplayNames.Add(TEXT("Buff_Slow"), FText::FromString(TEXT("이동 속도 감소")));

	HarmfulBuffIDs.Add(TEXT("Buff_Slow"));
}
