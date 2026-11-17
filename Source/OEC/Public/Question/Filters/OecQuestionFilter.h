// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/OecDataStruct.h"
#include "Data/OecQuestionTypes.h"
#include "OecQuestionFilter.generated.h"

/**
 * 질문 출제 필터 (베이스)
 *
 * - 출제 조건 1개 = 필터 클래스 1개. "이 질문을 지금 낼 수 있는가"만 답한다.
 * - UOecQuestionSubsystem이 시작할 때 하위 클래스를 모두 찾아 자동 등록한다.
 *   → 새 조건은 하위 클래스 하나만 추가하면 되고, 기존 코드는 수정하지 않는다.
 * - 필터는 상태를 갖지 않는다. 필요한 기록은 FOecQuestionContext로 받는다.
 */
UCLASS(Abstract)
class OEC_API UOecQuestionFilter : public UObject
{
	GENERATED_BODY()

public:
	// 이 질문을 지금 낼 수 있으면 true
	virtual bool PassesFilter(const FOecQuestionContext& InContext, const FQuestionStaticData& InQuestion) const PURE_VIRTUAL(UOecQuestionFilter::PassesFilter, return true;);

	// 후보가 하나도 없을 때 건너뛰어도 되는 필터인지 (연속 제한처럼 "되도록" 지키는 규칙)
	bool IsRelaxable() const { return bRelaxable; }

protected:
	// 하위 클래스 생성자에서 지정
	bool bRelaxable = false;
};
