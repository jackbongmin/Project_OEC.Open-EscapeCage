# 회차(반복) 시스템

작성: 2026-11-18 · 상태: 빌드 확인, **에디터 플레이 미확인**

주인공은 시뮬레이션 안에서 같은 고통을 반복한다 (GDD 1장). 사망·붕괴·탈출로 한 회차가 끝나면 메인 방에서 다음 회차가 시작된다.

---

## 1. 흐름

```
체력 0 / 정신력 0 (AOecPlayerCharacter가 어트리뷰트 변화를 구독)
   └→ UOecRunSubsystem.EndRun(사유)
        └→ OnRunEnded ──> AOecHUD: 종료 화면(UOecRunEndWidget) 표시, 마우스 표시, 이동·시점 입력 차단
                          "다시 시작" 버튼
   └→ UOecRunSubsystem.RequestNextRun()
        1. 회차 +1
        2. OnRunStarted ──> 각 서브시스템이 스스로 초기화
        3. 현재 레벨 다시 로드 (몬스터, 픽업, 문, 방, 플레이어 스탯이 처음 상태로)
```

## 2. 설계 포인트 (포트폴리오 설명용)

| 문제 | 설계 | 효과 |
|---|---|---|
| 회차가 바뀔 때 초기화할 시스템이 많고 계속 늘어난다 | 회차 서브시스템은 `OnRunStarted`만 발행하고, 각 서브시스템이 **구독해서 스스로 초기화** | 새 시스템 추가 시 회차 서브시스템 수정 없음. 무엇이 초기화되는지는 각 시스템이 안다 |
| 게임 인스턴스 서브시스템은 레벨이 바뀌어도 남는다 | "회차 단위 상태"(스택, 인벤토리, 지속 효과)는 초기화, "회차를 넘는 상태"(회차 수, 질문 풀 사용 기록, 전체 선택 기록)는 유지 | 반복할수록 새 질문이 나오고, 스토리는 회차마다 처음부터 |
| 월드 액터를 하나하나 되돌리기 어렵다 | 레벨을 다시 로드 | 몬스터·픽업·문·스트리밍 방이 자동으로 처음 상태 |
| 종료 화면 디자인이 아직 없다 | `UOecRunEndWidget`이 블루프린트가 없으면 **C++로 기본 화면을 직접 구성**, 블루프린트가 있으면 그쪽 위젯을 사용 (`BindWidgetOptional`) | 지금 바로 동작하고, 나중에 디자인만 교체 |
| 레벨이 바뀌면 HUD가 사라지는데 서브시스템 델리게이트는 남는다 | HUD `EndPlay`에서 구독 해제 | 이전 레벨 HUD로 가는 호출이 쌓이지 않음 |

## 3. 회차가 시작될 때 초기화되는 것

| 시스템 | 초기화 | 유지 |
|---|---|---|
| `UOecStageManagerSubsystem` | 페이즈 1, 전구 0 | - |
| `UOecEventSubsystem` | 열쇠 스택 = 설정 `InitialKeyStacks` | 질문 풀 사용 기록 |
| `UOecInventorySubsystem` | 빈 인벤토리 | - |
| `UOecQuickSlotSubsystem` | 빈 퀵슬롯 | - |
| `UOecChoiceSubsystem` | 지속 효과(답변마다 스택 확률, 방 어두워짐 등) | 전체 선택 기록 |
| `UOecQuestionSubsystem` | 회차 답변 기록, 스택 보정 카운트, 스토리 성향 횟수 | - |
| 월드 (레벨 다시 로드) | 플레이어 체력·정신력, 몬스터, 픽업, 문, 방 | - |

## 4. 설정 (프로젝트 설정 › OEC Game Data Settings)

| 항목 | 기본값 | 뜻 |
|---|---|---|
| Stage › Initial Key Stacks | 3 | 회차 시작 시 열쇠 스택 (GDD "논의 필요" 항목, 기존 테스트값 유지) |
| Stage › Run Auto Restart Delay | 0 | 0 = "다시 시작" 버튼을 누를 때까지 대기. 0보다 크면 그 시간 뒤 자동으로 다음 회차 |
| Question › Sanity Change Per Answer | -1 | 답변할 때마다 정신력 변화 (GDD 3.2. 이번에 처음 코드로 들어감) |

## 5. 종료 화면 (`UOecRunEndWidget`)

- 기본 화면: 검은 반투명 배경, 빨간 제목(사망 / 붕괴 / 탈출), WARDEN 대사, "다시 시작", "게임 종료" 버튼.
- 문구 바꾸기: 위젯 블루프린트의 클래스 디폴트 (Class Defaults) › **Title Texts**, **Message Texts**. 메시지의 `{0}`은 끝난 회차 번호.
- 디자인 교체: `UOecRunEndWidget`을 부모로 위젯 블루프린트(예: `WBP_RunEnd`)를 만들고 `Txt_Title`, `Txt_Message`, `Btn_Restart`, `Btn_Quit` 이름으로 배치 → HUD 블루프린트의 **Run End Widget Class**에 지정. 연출은 `BP_OnRunEndInfoSet` 이벤트에서.

## 6. 치트

| 명령 | 설명 |
|---|---|
| `OecStartNewRun` | 종료 화면 없이 바로 다음 회차 (레벨 다시 로드) |
| `OecSetHealth 0` | 사망 → 종료 화면 확인 |
| `OecSetSanity 0` | 붕괴 → 종료 화면 확인 |

## 7. 동작 확인 (사용자)
1. 플레이 → 아이템을 몇 개 줍고 전구를 1개 이상 켠다.
2. 콘솔 `OecSetHealth 0` → "사망" 화면, 마우스 커서 표시, 이동 불가 확인.
3. "다시 시작" → 메인 방에서 다시 시작, 튜토리얼 1번 질문, 인벤토리·퀵슬롯·전구가 비어 있는지 확인.
4. `OecShowState` → 2회차, 열쇠 3, 정신력 80 확인.
5. 질문에 답할 때마다 정신력이 1씩 줄어드는지 확인.

## 8. 알려진 한계
- **탈출 엔딩은 아직 연결하지 않았다.** `EndRun(Escape)`를 탈출 방에서 부르면 같은 흐름으로 동작한다.
- **진엔딩 판정**(기억 조각 + 스토리 성향)은 없다. 스토리 성향 횟수는 `UOecQuestionSubsystem::GetStoryTagCount`로 읽을 수 있다.
- 레벨 다시 로드는 동기 로드라 화면이 잠깐 멈출 수 있다. 필요하면 로딩 화면이나 페이드를 붙인다.
