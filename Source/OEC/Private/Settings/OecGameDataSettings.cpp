// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/OecGameDataSettings.h"

UOecGameDataSettings::UOecGameDataSettings()
{
	// 손해 질문은 2연속 금지 (QuestionGuideline.md 2.3)
	QuestionTypeStreakLimits.Add(EQuestionType::Penalty, 1);

	// 튜토리얼과 스토리는 회차마다 처음부터 다시 나온다
	RunScopedQuestionTypes.Add(EQuestionType::Tutorial);
	RunScopedQuestionTypes.Add(EQuestionType::Story);
}
