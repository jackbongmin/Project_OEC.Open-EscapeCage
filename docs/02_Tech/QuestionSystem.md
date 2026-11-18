# 질문 출제 시스템

작성: 2026-11-17 · 상태: 빌드 확인, **에디터 플레이 미확인**

"다음에 어떤 질문을 낼지"를 정하는 시스템이다. 선택한 뒤의 결과 처리는 [ChoiceSystem.md](ChoiceSystem.md), 질문 데이터 기준은 [QuestionGuideline.md](../01_Design/QuestionGuideline.md) 9장.

---

## 1. 구조 한눈에 보기

```
AOecTerminalBase ──(테이블 + 현재 페이즈의 타입 가중치)──> UOecQuestionSubsystem.PickQuestion()
                                                            1. 필터로 후보 걸러내기      UOecQuestionFilter_*
                                                            2. 선택기를 우선순위대로 시도  UOecQuestionSelector_*
                                                            3. 후보가 없으면 완화 → 사용 기록 초기화 후 재시도

UOecChoiceSubsystem.OnChoiceResolved ─┐
UOecStageManagerSubsystem.OnStackChanged ─┼─(구독)─> UOecQuestionSubsystem (회차 기록, 스택 보정 카운트)
UOecStageManagerSubsystem.OnKeyRewarded ─┘ (페이즈 2 스택 보상 = 열쇠 지급)
```

- 단말기는 "무엇을 낼지" 모른다. 테이블과 가중치를 넘기고 받은 질문을 화면에 띄우기만 한다.
- 질문 서브시스템은 답변과 스택 변화를 **델리게이트로 구독**해서 기록을 쌓는다. 선택지 서브시스템은 질문 서브시스템을 모른다 (Architecture.md 2장).

## 2. 설계 포인트 (포트폴리오 설명용)

| 문제 | 설계 | 효과 |
|---|---|---|
| 출제 규칙이 단말기 함수 하나에 뭉쳐 있었다 | 규칙을 **필터**(낼 수 있는가)와 **선택기**(무엇을 고를까)로 나누고 서브시스템이 조립 | 단말기는 표시만 담당. 규칙 하나 = 클래스 하나 |
| 규칙이 계속 늘어난다 (페이즈 제한, 연쇄, 보정 ...) | `GetDerivedClasses`로 하위 클래스 **자동 등록** (선택지 효과 실행기와 같은 방식) | 새 규칙 = 클래스 추가. 서브시스템·단말기 수정 없음 |
| 튜토리얼·스택 보정처럼 "무작위보다 먼저" 적용할 규칙 | 선택기에 **우선순위**를 두고 높은 것부터 시도, `nullptr`이면 다음으로 | 예외 규칙을 `if` 사슬 없이 표현 |
| 연속 제한을 지키다 낼 질문이 없어질 수 있다 | 필터에 **완화 가능** 표시. 후보가 없으면 완화 가능 필터만 풀고 재시도 | "되도록 지키는 규칙"과 "반드시 지키는 규칙"을 구분 |
| 수치가 코드에 박힌다 | 보정 횟수, 연속 제한, 회차 단위 타입을 **프로젝트 설정**으로 | 밸런스 조정에 빌드가 필요 없음 |
| 스토리 연쇄는 회차 안에서만 의미가 있다 | 튜토리얼·스토리는 **회차 단위 기록**으로, 나머지는 풀 소진 기록으로 중복 방지 | 회차마다 스토리가 처음부터 다시 진행 (반복 설정) |

## 3. 필터 (`Source/OEC/Public/Question/Filters/`)

| 클래스 | 조건 | 데이터 필드 | 완화 |
|---|---|---|---|
| `_Enabled` | 꺼 둔 질문 제외 | `bEnabled` | X |
| `_Phase` | 현재 페이즈에서 낼 수 있는 질문만 | `AllowedPhases` (비면 전체) | X |
| `_RunCount` | N회차부터 출제 | `MinRunCount` | X |
| `_Used` | 중복 출제 방지 (회차 단위 타입은 이번 회차 기록, 그 외는 풀 소진 기록) | 설정 `RunScopedQuestionTypes` | X |
| `_Prerequisite` | 선행 질문에 특정 버튼으로 답했어야 출제 (하나만 맞아도 됨) | `RequiredChoices` | X |
| `_TypeStreak` | 같은 타입이 제한 횟수 넘게 연달아 나오지 않음 | 설정 `QuestionTypeStreakLimits` | O |

## 4. 선택기 (`Source/OEC/Public/Question/Selectors/`)

| 우선순위 | 클래스 | 동작 |
|---|---|---|
| 300 | `_FixedOrder` | `FixedOrder`가 0보다 큰 질문이 후보에 있으면 작은 번호부터 낸다 (튜토리얼). 가중치와 무관 |
| 200 | `_StackPity` | 스택 없이 N회 연속 답했으면, 어느 한쪽 선택지가 스택을 100% 주는 질문을 낸다 |
| 100 | `_Weighted` | 단말기 가중치로 타입을 뽑고 그 안에서 무작위 (기존 방식) |

새 규칙 추가 순서
1. `UOecQuestionFilter` 또는 `UOecQuestionSelector`를 상속한 클래스를 만든다.
2. 필터는 `PassesFilter`, 선택기는 생성자에서 `Priority` 지정 후 `SelectQuestion`을 구현한다.
3. 끝. 등록은 자동이다.

## 5. 데이터와 설정

**`FQuestionStaticData`에 추가된 필드** (JSON에서 그대로 임포트)

| 필드 | 뜻 | 기본값 |
|---|---|---|
| `bEnabled` | 꺼 두면 출제 안 함 | `true` |
| `AllowedPhases` | 출제 가능한 페이즈 번호 | 빈 배열 = 전체 |
| `RequiredChoices` | `[{ QuestionID, Button }]`, `Button`은 `Red` / `Blue` | 빈 배열 |
| `MinRunCount` | 이 회차부터 출제 | 0 |
| `FixedOrder` | 고정 순서 (0 = 무작위 출제) | 0 |
| `BalanceNote` | 검수용 메모 | - |
| 선택지 안 `StoryTag` | `None` / `Admit` / `Deny` / `Avoid` | `None` |

**프로젝트 설정 › OEC Game Data Settings › Question**

| 항목 | 기본값 | 뜻 |
|---|---|---|
| Stack Pity Answer Count | 5 | 이 횟수만큼 연속으로 스택을 못 얻으면 확정 스택 질문 보장 (0 = 끔) |
| Default Question Type Streak Limit | 2 | 같은 타입 최대 연속 횟수 (= 3연속 금지) |
| Question Type Streak Limits | 패널티 1 | 타입별 예외 (= 손해 2연속 금지) |
| Run Scoped Question Types | 튜토리얼, 스토리 | 회차마다 다시 나오는 타입 |

**기록**: `FOecChoiceRecord`에 `Button`(어느 버튼)과 `StoryTag`가 추가됐다. `UOecQuestionSubsystem::GetStoryTagCount`로 이번 회차의 성향별 횟수를 읽을 수 있다 (엔딩 판정용).

## 6. 치트 커맨드

| 명령 | 설명 |
|---|---|
| `OecShowState` | 기존 출력에 더해 회차, 스택 없이 연속 답변한 횟수, 스토리 성향 횟수 출력 |
| `OecStartNewRun` | 새 회차 시작 (튜토리얼·스토리가 다시 나오고 회차 수 +1). 회차 관리는 [RunSystem.md](RunSystem.md) |

---

## 7. 에디터 연결 가이드 (사용자 작업)

> 정확한 한글 표기가 다르게 보이면 디테일 (Details) 패널 검색창에 영문 이름을 입력해 찾는다.

### 7.1 질문 테이블 다시 임포트
- 콘텐츠 브라우저 (Content Browser)에서 `DT_QuestionStaticData` 우클릭 → **새 파일로 리임포트 (Reimport With New File)** → 파일 형식을 **JSON** 또는 **모든 파일**로 바꾸고 `ProjectData/QuestionStaticData_All.json` 선택.
- 메뉴가 없으면 [ChoiceSystem.md](ChoiceSystem.md) 6.3의 방법 B를 쓴다.
- 임포트 후 행이 207개인지, `Q_TU_001`의 고정 순서 (Fixed Order)가 1인지 확인한다.

### 7.2 단말기 가중치
맵에 배치한 단말기를 선택 → 디테일 (Details) › **OEC | Data**

| 타입 | Question Type Weights (페이즈 1) | Phase 2 Question Type Weights |
|---|---|---|
| 보너스 | 11 | 9 |
| 패널티 | 13 | 11 |
| 밸런스 | 15 | 12 |
| 도박 | 16 | 13 |
| 이벤트 | 10 | 15 |
| 무의미 | 20 | 15 |
| 스토리 | 15 | 25 |

- **튜토리얼은 넣지 않는다.** 고정 순서 선택기가 가중치와 무관하게 먼저 낸다.
- 이벤트는 켜진 질문이 아직 없으므로 넣어도 나오지 않는다 (가중치는 나머지 타입에 자동으로 나뉜다).
- 수치의 근거는 [QuestionBalanceReport.md](../01_Design/QuestionBalanceReport.md) 6.2 (가이드라인 2.3의 합계를 질문 개수 비율로 나눈 값).

### 7.3 동작 확인
1. 플레이 → 첫 질문이 "버튼을 눌러 응답하십시오..."(튜토리얼 1), 다음이 "선물을 드리죠."(튜토리얼 2)인지 확인.
2. 출력 로그 (Output Log)에서 `질문 [Q_...] 출제 (OecQuestionSelector_..., 후보 N개)` 줄로 어떤 선택기가 골랐는지 확인.
3. 스택 없는 선택만 5번 한 뒤 `스택 보정 발동` 로그와 확정 스택 선택지가 나오는지 확인.
4. 콘솔에 `OecSetPhase 2` → 페이즈 2 전용 질문(예: 죽음의 도박)이 나오기 시작하는지 확인.
5. 스토리 도입(`Q_ST_A1`)에 답한 뒤에만 `Q_ST_A2a` 또는 `A2b`가 나오는지 확인. `OecShowState`로 성향 횟수 확인.
6. `OecStartNewRun` → 튜토리얼이 다시 나오는지 확인.

## 8. 알려진 한계
- ~~회차 시작 연결 없음~~ → 2026-11-18 회차 시스템으로 연결 ([RunSystem.md](RunSystem.md)). 회차 수는 `UOecRunSubsystem`이 관리하고, 질문 서브시스템은 `OnRunStarted`를 구독해 회차 기록을 초기화한다.
- **튜토리얼 3번은 좀비·응급 붕대 데이터 행이 있어야 동작한다** (2026-11-18 켬). 행이 없으면 출제는 되지만 소환·지급이 실패한다. [ChoiceSystem.md](ChoiceSystem.md) 7장.
- **스택 보정은 페이즈 구분 없이 동작**한다. 페이즈 2(열쇠)에서 끄려면 설정이나 선택기를 손봐야 한다.
- **두 단말기가 같은 질문을 동시에 띄울 수 있다** (기존과 같음). 한쪽에서 답하면 다른 쪽은 버튼을 누를 때 새 질문으로 바뀐다.
- **무의미 질문을 손해·도박 직후에 우대**하는 규칙(가이드라인 2.3)은 넣지 않았다. 필요하면 선택기 하나로 추가할 수 있다.
