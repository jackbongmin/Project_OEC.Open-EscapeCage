# 방 이벤트 시스템 · 이벤트 아이템

작성: 2026-11-18 · 상태: 빌드 확인, **에디터 플레이 미확인** · 기획: [QuestionGuideline.md](../01_Design/QuestionGuideline.md) 3.3, 5장

질문 결과(`Effect.Event.Start`)로 메인 방에 구조물·위험물이 생기는 이벤트와, 퀵슬롯에서 쓰는 이벤트 아이템 5종.

---

## 1. 구조

```
질문 결과 Effect.Event.Start (Param = 이벤트 ID 또는 Random)
   └→ UOecChoiceEffect_StartEvent
        1. DT_RoomEvent에서 데이터 조회 (FOecRoomEventData)
        2. 이벤트 기준점(OecChoiceSpawnPoint, 타입 Event)에 이벤트 액터 생성
        3. AOecRoomEventBase::StartEvent
             시작 효과 실행 → OnEventStarted (하위 클래스가 방에 무언가를 만듦)
             종료: 시간 / 답변 수 / 페이즈 전환 / CompleteEvent
             성공·실패 효과 실행 (선택지 효과 구조 재사용) → 정리
```

## 2. 설계 포인트 (포트폴리오 설명용)

| 문제 | 설계 | 효과 |
|---|---|---|
| 이벤트마다 시간·종료 조건·보상이 다르다 | **데이터**(`DT_RoomEvent`)가 지속 시간, 답변 수, 성공 조건, 시작·성공·실패 효과를 정하고, 액터는 "방에서 무슨 일이 일어나는지"만 구현 | 수치·보상 조정에 코드 수정 없음. 같은 액터로 보상형/기본형 이벤트 (튀는 구체) |
| 보상과 피해를 이벤트마다 따로 구현하면 GE·클램프를 우회하기 쉽다 | 보상·피해를 **선택지 효과(FOecChoiceEffect)** 로 표현해 같은 실행기로 실행 | 체력 피해도 GE + SetByCaller, 보상도 질문과 같은 문구·로직 |
| "정전"처럼 이미 있는 효과로 되는 이벤트 | 베이스 클래스를 그대로 쓰고 **시작 효과**에 `Effect.Env.Darken` | 새 클래스 없이 데이터만으로 이벤트 추가 |
| 블루프린트 디자인 전에도 테스트하고 싶다 | 엔진 기본 도형(구, 원기둥, 상자, 평면)과 색으로 **기본 표시**를 C++에 둠. 데이터는 C++ 클래스 경로를 직접 가리킴 | 에디터 작업 없이 동작. 디자인은 블루프린트 하위 클래스로 교체 |
| 아이템 효과가 "지금은 못 쓰는" 경우가 있다 (스토리 질문에 질문 변경권) | 실행기에 `CanExecute`, 아이템 어빌리티가 `ShouldAbilityRespondToEvent`에서 검사 | 못 쓰면 어빌리티가 반응하지 않아 **아이템이 소모되지 않음** |
| 메모·보험·소음 차단기처럼 "나중에 한 번" 발동하는 효과 | **충전식 지속 효과**(AnswerModifier, Value = 남은 횟수)로 저장, 필요한 시스템이 `ConsumeAnswerModifier`로 1 소모 | 상태 아이콘 UI에 그대로 표시 가능. 새 충전 효과 = 하위 클래스 1개 |
| 보험이 "나쁜 결과"를 판정해야 한다 | 실행기마다 `IsHarmful` (체력·정신력 감소, 즉사, 몬스터, 어두워짐, 해로운 버프·이벤트) | 새 효과도 자기 해로움만 선언하면 됨 |

## 3. 이벤트 (페이즈 1용 "쉬움" 7종 + 보상형 1)

| ID | 클래스 | 내용 (기본 표시) | 종료 | 성공 조건 → 보상 |
|---|---|---|---|---|
| `Event_BouncingOrbs` | `OecRoomEvent_BouncingOrbs` | 하늘색 구 5개가 벽에 튕김, 닿으면 체력 -5 (1초 쿨다운) | 30초 | 보상 없음 |
| `Event_BouncingOrbs_Reward` | 위와 같음 | 위와 같음 | 30초 | 끝까지 살아 있으면 스택 +1 |
| `Event_RisingPillar` | `OecRoomEvent_RisingPillar` | 회색 원기둥(지름 220, 높이 160)이 솟음. 점프로 올라감 | 20초 후 내려감 | - |
| `Event_CoverWalls` | `OecRoomEvent_CoverWalls` | 벽 3개(300×40×200)가 솟음 | 페이즈가 바뀔 때 | - |
| `Event_FallingDebris` | `OecRoomEvent_FallingDebris` | 바닥 붉은 원 → 1초 뒤 잔해 낙하, 10회 (절반은 플레이어 근처) | 15초 | 한 번도 안 맞으면 응급 붕대 |
| `Event_GasZone` | `OecRoomEvent_DamageZone` | 기준점 한쪽의 초록 바닥(1000×600) 안에 있으면 초당 체력 -2 | 20초 | - |
| `Event_Blackout` | `OecRoomEventBase` | 방 어두워짐 (시작 효과) | 답변 3번 | - |
| `Event_LowGravity` | `OecRoomEvent_GravityShift` | 중력 0.35배 | 30초 | - |

- 수치(개수, 속도, 피해, 범위)는 각 클래스의 디테일 (Details) › OEC | Event에서 바꾼다. 블루프린트 하위 클래스를 만들고 데이터의 `EventActorClass`를 그 블루프린트로 바꾸면 된다.
- 연출: `BP_OnEventStarted`, `BP_OnEventFinished(성공 여부)`, `BP_OnPlayerHit(피해량)`.
- 솟는 구조물은 컴포넌트 태그 `OecRise`가 붙은 메시를 함께 움직인다. 블루프린트에서 메시를 추가하고 태그만 붙이면 된다.
- 보통 난이도 5종(보급 상자 방어, 레이저 스윕, 전기 바닥, 천장 터렛, 압력판)은 페이즈 2용이라 아직 없다. 쓰는 질문은 꺼져 있다.

## 4. 이벤트 아이템 5종

| 아이템 | 효과 태그 | 동작 | 못 쓰는 경우 |
|---|---|---|---|
| `EV_ChaosCube` 불안정한 큐브 | `Effect.Event.Start` (Param `Random`) | 현재 페이즈에서 나올 수 있는 이벤트 중 무작위 1개 (보상형 구체 제외) | 후보 이벤트가 없을 때 |
| `EV_Reroll` 질문 변경권 | `Effect.Question.Reroll` | 지금 단말기 질문을 다른 질문으로 교체 (지금 질문은 바로 다음 출제에서 제외) | 스토리·튜토리얼 질문일 때 |
| `EV_Peek` WARDEN의 메모 | `Effect.Question.Peek` | 다음 **확률 선택지가 있는 질문**의 결과를 미리 굴려 버튼에 "[메모] 확정: ..."으로 표시. 고르면 그 결과 그대로 | - |
| `EV_Insurance` 보험 증서 | `Effect.Guard.Insurance` | 다음 확률 결과가 해로우면 한 번 다시 굴림 | - |
| `EV_Silencer` 소음 차단기 | `Effect.Guard.Silencer` | 다음 몬스터 소환 효과 1회를 통째로 무효 | - |

- 메모·보험·소음 차단기는 충전식이라 여러 개 쓰면 횟수가 쌓인다. `OecShowState`에 남은 횟수가 나온다.
- 가이드라인 원안은 메모가 "다음 도박 질문"이지만, 질문 타입에 묶지 않고 **확률 선택지가 있는 다음 질문**으로 넓혔다 (교환 질문에도 확률 선택지가 있음).

## 5. 에디터 작업 (사용자)

### 5.1 이벤트 데이터 테이블
1. 윈도우 탐색기에서 `ProjectData/RoomEventData.json`을 콘텐츠 브라우저의 `05_Data` 폴더로 끌어다 놓는다.
2. 데이터 테이블 옵션 (DataTable Options) 창에서 행 타입 (Row Type)을 **OecRoomEventData**로 고르고 적용 (Apply) → 이름을 `DT_RoomEvent`로.
3. 프로젝트 설정 › OEC Game Data Settings › DataTables › **Room Event Data Table**에 `DT_RoomEvent` 지정.

### 5.2 이벤트 기준점
- 메인 방 맵에 `OecChoiceSpawnPoint`를 하나 더 놓고 디테일 › Spawn Type을 **이벤트 기준점 (방 중앙 바닥)**, Spawn Radius를 **0**으로. 방 한가운데 바닥 근처에 둔다.
- 없으면 플레이어 위치에 생기고 경고 로그가 남는다.

### 5.3 이벤트 아이템 행 (`DT_ItemStaticData`)
| 행 이름 (= Item Code) | Item Name | Item Type | Use Effects (소모품 칸) |
|---|---|---|---|
| `EV_ChaosCube` | 불안정한 큐브 | 소모품 (Consumable) | `Effect.Event.Start`, Value 1, Param `Random` |
| `EV_Reroll` | 질문 변경권 | 소모품 | `Effect.Question.Reroll`, Value 1 |
| `EV_Peek` | WARDEN의 메모 | 소모품 | `Effect.Question.Peek`, Value 1 |
| `EV_Insurance` | 보험 증서 | 소모품 | `Effect.Guard.Insurance`, Value 1 |
| `EV_Silencer` | 소음 차단기 | 소모품 | `Effect.Guard.Silencer`, Value 1 |

- 체력·정신력 회복량은 0. 픽업은 비워 두면 기본 픽업.

### 5.4 질문 테이블 다시 임포트
- `ProjectData/QuestionStaticData_All.json` (쉬움 이벤트와 이벤트 아이템을 쓰는 질문이 켜졌다. 켜진 질문 187개).
- 단말기의 Question Type Weights에 **이벤트** 가중치(페이즈 1: 10)가 들어 있는지 확인한다.

## 6. 동작 확인
| 명령 / 행동 | 기대 결과 |
|---|---|
| `OecRunEffect Effect.Event.Start 1 Event_BouncingOrbs` | 구 5개가 튕기고, 닿으면 체력 -5, 30초 뒤 사라짐 |
| `OecRunEffect Effect.Event.Start 1 Event_FallingDebris` | 붉은 원 → 잔해 낙하 10회. 다 피하면 응급 붕대 투하 |
| `OecRunEffect Effect.Event.Start 1 Event_RisingPillar` | 기둥이 솟고 20초 뒤 내려감 |
| `OecRunEffect Effect.Event.Start 1 Event_Blackout` | 방이 어두워지고 답변 3번 뒤 밝아짐 |
| `OecRunEffect Effect.Event.Start 1 Random` | 무작위 이벤트 |
| `OecGiveItem EV_Reroll` → 퀵슬롯 사용 | 단말기 질문이 바뀜. 스토리 질문일 때는 바뀌지 않고 아이템도 그대로 |
| `OecGiveItem EV_Peek` → 사용 | 다음 확률 질문 버튼에 "[메모] 확정: ..." |
| `OecGiveItem EV_Silencer` → 사용 → 몬스터 소환 결과 선택 | 로그 "소음 차단기로 소환이 무효", 몬스터 없음 |

## 7. 알려진 한계
- 이벤트끼리 겹칠 수 있다 (예: 독가스 + 낙하 잔해). 의도된 조합 질문도 있어서 막지 않았다.
- 엄폐 벽·기둥은 내비게이션 메시를 다시 만들지 않는다. 몬스터가 벽에 막혀 비비는 모습이 보이면 프로젝트 설정 › 내비게이션 메시 (Navigation Mesh) › 런타임 생성 (Runtime Generation)을 **Dynamic**으로 바꾼다.
- 메모로 미리 굴린 결과에는 보험이 적용되지 않는다 (이미 확정된 결과).
