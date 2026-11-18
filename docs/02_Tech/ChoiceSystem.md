# 선택지 효과 시스템 (M1)

질문 버튼을 눌렀을 때의 결과를 **데이터만으로** 표현하고 실행하는 시스템이다.
질문지를 나중에 한 번에 작성해도 코드를 다시 고칠 필요가 없도록 만드는 것이 목표다.

---

## 1. 구조 한눈에 보기

```
[DT_QuestionStaticData]  ── JSON에서 임포트
   └ RedChoice / BlueChoice (FOecChoiceData)
        └ Outcomes[] (가중치)
             └ Effects[] (태그 + Value + Param)

AOecTerminalBase ──(질문 선택)──> AOecQuestionButton.SetChoiceData()
        │                               │ E 키
        │ BuildChoiceText()             ▼
        └──────────────> UOecChoiceSubsystem.SubmitChoice()
                            1. 질문 사용 처리 (UOecEventSubsystem)
                            2. 지속 효과 진행 (AnswerModifier 틱/만료)
                            3. 결과 뽑기 (가중치)
                            4. 효과 실행 ──> 태그별 실행기 (UOecChoiceEffect_*)
                            5. 기록 + OnChoiceResolved 브로드캐스트

실행기 → 월드: UOecSpawnPointSubsystem (스폰 위치), UOecPoolingSubsystem (몬스터)
실행기 → 상태: AddAnswerModifier → OnAnswerModifierChanged → AOecRoomEnvironmentActor (조명 등)
```

## 2. 설계 포인트 (포트폴리오 설명용)

| 문제 | 설계 | 효과 |
|---|---|---|
| 효과 종류가 계속 늘어난다 | **전략 패턴 + 자동 등록 레지스트리**: 효과 1개 = `UOecChoiceEffectExecutor` 하위 클래스 1개. 서브시스템이 시작할 때 `GetDerivedClasses`로 찾아 `EffectTag`로 등록 | 새 효과 = 클래스 하나 추가. 서브시스템·버튼·단말기 수정 없음 |
| "50% 스택 아니면 50% 체력 -30" 같은 확률 분기 | 선택지 → **결과(가중치)** → 효과 목록의 3단 데이터 구조 | 즉사 확률, 복합 효과, 꽝을 전부 데이터로 표현 |
| "앞으로 답변할 때마다 1%" 같은 지속 효과 | 답변 단위 **AnswerModifier** (영구/기간제, 누적/갱신) + 실행기의 `OnAnswerTick` / `OnModifierExpired` 훅 | 지속 효과도 같은 실행기 구조로 확장 |
| 버튼 문구를 매번 손으로 써야 한다 | 실행기마다 `BuildDescription` → 서브시스템이 결과·확률을 조합해 문구 생성 (`"50% 확률로 스택 +1 / 50% 확률로 체력 30 감소"`) | 효과 질문은 문구 작성 불필요. 밈 질문만 직접 작성 |
| 서브시스템이 월드 액터를 직접 찾으면 결합도가 높다 | 스폰 지점은 **스스로 월드 서브시스템에 등록**, 환경 연출은 **델리게이트 구독** | 계층 의존 방향 유지 (Architecture.md 2장) |
| 스탯 변화가 클램프를 우회할 수 있다 | 체력·정신력은 반드시 **GE + SetByCaller(효과 태그)** 로 적용 | `PostGameplayEffectExecute` 클램프를 항상 통과 |
| 테스트할 때마다 질문을 기다려야 한다 | **치트 커맨드** (`OecRunEffect` 등) | 모든 효과를 콘솔에서 바로 실행 |

## 3. 효과 목록

| 태그 | 실행기 | Value | Param | 문구 예 |
|---|---|---|---|---|
| `Effect.None` | `_None` | - | - | 아무 일도 일어나지 않음 |
| `Effect.Stack` | `_Stack` | 증감량 | - | 페이즈 1: 스택 +1 (전구) / 페이즈 2: 열쇠 1개 획득 (열쇠 아이템 투하) |
| `Effect.Stat.Health` | `_Health` | 증감량 | - | 체력 10 회복 |
| `Effect.Stat.Sanity` | `_Sanity` | 증감량 | - | 정신력 10 감소 |
| `Effect.Death` | `_Death` | - | - | 즉시 사망 |
| `Effect.Buff` | `_Buff` | 지속 답변 수 | 버프 ID | 이동 속도 증가 (답변 5번 동안) |
| `Effect.Spawn.Monster` | `_SpawnMonster` | 마리 수 | 몬스터 코드 / `Random` | 골렘 2마리 소환 |
| `Effect.Give.Item` | `_GiveItem` | 수량 | 아이템 코드 | 회복약 획득 |
| `Effect.Passive.AnswerStackChance` | `_AnswerStackChance` | 확률 (0.01 = 1%) | - | 앞으로 답변할 때마다 1% 확률로 스택 +1 |
| `Effect.Passive.AnswerHealth` | `_AnswerHealth` | 1회 변화량 | 지속 답변 수 (예: `10`) | 답변 10번 동안 매번 체력 +2 |
| `Effect.Passive.AnswerSanity` | `_AnswerSanity` | 1회 변화량 | 지속 답변 수 | 답변 10번 동안 매번 정신력 -1 |
| `Effect.Event.Start` | `_StartEvent` | - | 이벤트 ID / `Random` | 튀는 구체 5개 (30초) ([EventSystem.md](EventSystem.md)) |
| `Effect.Question.Reroll` | `_QuestionReroll` | - | - | 지금 질문 교체 |
| `Effect.Question.Peek` | `_PeekOutcome` (충전식) | 충전 횟수 | - | 다음 확률 결과 미리 보기 |
| `Effect.Guard.Insurance` | `_Insurance` (충전식) | 충전 횟수 | - | 나쁜 확률 결과 1회 다시 굴림 |
| `Effect.Guard.Silencer` | `_Silencer` (충전식) | 충전 횟수 | - | 다음 몬스터 소환 1회 무효 |
| `Effect.Env.Darken` | `_Darken` | 지속 답변 수 (0 = 영구) | - | 방이 어두워짐 (답변 3번 동안) |

새 효과 추가 순서
1. `OecGameplayTags.h/.cpp`에 `Effect.*` 태그 추가
2. `Choice/Effects/`에 `UOecChoiceEffect_이름` 클래스 추가 (생성자에서 `EffectTag` 지정, `Execute`·`BuildDescription` 구현)
3. 끝. 등록은 자동이다.

## 4. 질문 데이터 (JSON)
원본: [`ProjectData/QuestionStaticData.json`](../../ProjectData/QuestionStaticData.json) (테스트 질문 10개)

```json
{
	"Name": "Q_Test_05",
	"QuestionID": "Q_Test_05",
	"QuestionType": "Balance",
	"QuestionText": "둘 중에 고르시오",
	"RedChoice": {
		"DisplayText": "",
		"Outcomes": [
			{ "Weight": 50, "Effects": [ { "EffectTag": { "TagName": "Effect.Stack" }, "Value": 1, "Param": "None" } ] },
			{ "Weight": 50, "Effects": [ { "EffectTag": { "TagName": "Effect.Stat.Health" }, "Value": -30, "Param": "None" } ] }
		]
	},
	"BlueChoice": { "DisplayText": "그냥 넘어간다", "Outcomes": [] }
}
```
- `DisplayText`가 비어 있으면 문구 자동 생성.
- `Outcomes`가 비어 있으면 효과 없는 선택지 (무의미 질문).
- `Name`과 `QuestionID`는 같은 값으로 둔다 (중복 출제 방지에 `QuestionID` 사용).
- 기존 `QuestionStaticData.csv`는 이전 구조라 더 이상 쓰지 않는다.

## 5. 치트 커맨드
에디터 플레이 중 `` ` `` 키로 콘솔을 열고 입력한다. (쉬핑 빌드에서는 비활성)

| 명령 | 설명 |
|---|---|
| `OecAddStack 5` | 현재 페이즈 스택 +5 (페이즈 1이면 페이즈 2로 전환) |
| `OecSetPhase 2` | 페이즈 변경 |
| `OecSetHealth 50` / `OecSetSanity 10` | 스탯 직접 설정 |
| `OecSpawnMonster Golem_N 2` | 몬스터 소환 (코드 생략 시 무작위) |
| `OecGiveItem Heal30` | 아이템 지급 |
| `OecRunEffect Effect.Env.Darken 3` | 아무 효과나 실행 |
| `OecShowState` | 페이즈, 스택, 체력, 정신력, 지속 효과 출력 |

---

## 6. 에디터 연결 가이드 (사용자 작업)

> 코드 변경이 많으니 **에디터를 닫고 빌드한 뒤** 진행한다.

### 6.1 GE 2개 만들기 (`Content/01_OEC/07_GAS/GE/`)
| 에셋 | 설정 |
|---|---|
| `GE_Choice_Health` | Duration Policy: **Instant** / Modifier: Attribute `OecAttributeSetBase.Health`, Op `Add`, Magnitude Type **Set by Caller**, Data Tag **`Effect.Stat.Health`** |
| `GE_Choice_Sanity` | Duration Policy: **Instant** / Modifier: Attribute `OecPlayerAttributeSet.Sanity`, Op `Add`, Magnitude Type **Set by Caller**, Data Tag **`Effect.Stat.Sanity`** |

### 6.2 프로젝트 설정 (Edit → Project Settings → Game → **OEC Game Data Settings**)
| 항목 | 값 |
|---|---|
| Stage › Phase1 Max Stack | 5 (전구 개수) |
| Choice › Choice Health Effect | `GE_Choice_Health` |
| Choice › Choice Sanity Effect | `GE_Choice_Sanity` |
| Choice › Default Pickup Class | 기본 픽업 블루프린트 (예: `BP_Pickup_Test`) |
| Choice › Choice Buff Effects | (선택) 버프 ID → 버프 GE |

### 6.3 질문 데이터 테이블 다시 임포트
- 구조체가 바뀌어서 `DT_QuestionStaticData`의 기존 행은 버튼 데이터가 비게 된다.
- **방법 A**: 콘텐츠 브라우저 (Content Browser)에서 `DT_QuestionStaticData` 우클릭 → **새 파일로 리임포트 (Reimport With New File)** → 파일 선택 창 오른쪽 아래 파일 형식을 **JSON** 또는 **모든 파일**로 바꾸고 `ProjectData/QuestionStaticData.json` 선택.
- **방법 B** (A 메뉴가 없을 때): 윈도우 탐색기에서 `QuestionStaticData.json`을 콘텐츠 브라우저의 `05_Data` 폴더로 끌어다 놓기 → **데이터 테이블 옵션 (DataTable Options)** 창에서 행 타입 (Row Type)을 `QuestionStaticData`로 고르고 **적용 (Apply)** → 새 데이터 테이블이 생김.
  - 이 경우 새 테이블을 두 곳에 연결해야 한다: 프로젝트 세팅의 Question Data Table, 맵에 배치한 단말기의 디테일 (Details) › Question Data Table.
- 단말기 `QuestionTypeWeights`에 다섯 유형(스토리, 보너스, 패널티, 밸런스, 무의미)이 모두 들어 있는지 확인한다. 가중치가 없는 유형의 질문은 나오지 않는다.

### 6.4 메인 방 배치
| 액터 | 위치 | 설정 |
|---|---|---|
| `OecChoiceSpawnPoint` (몬스터) | 천장 몬스터 스폰구 | Spawn Type: **몬스터 스폰구**, Spawn Radius |
| `OecChoiceSpawnPoint` (아이템) | 천장 아이템 투하구 | Spawn Type: **아이템 투하구** |
| `OecRoomEnvironmentActor` | 방 아무 곳 | 디테일 › Darken Lights에 방 조명을 스포이드로 지정, Darken Intensity Scale(어두워질 때 밝기 배율, 기본 0.1) |

- 스폰 지점은 여러 개 배치하면 무작위로 하나가 골라진다.
- 해치 열림, 이펙트 같은 연출은 스폰 지점을 블루프린트로 상속해 `BP_OnSpawned`에서 처리한다.
- 방 어두워짐은 C++에서 처리한다. 지정한 조명의 밝기를 배율만큼 낮췄다가 효과가 끝나면 원래 밝기로 되돌린다. 조명의 모빌리티 (Mobility)는 스테이셔너리 (Stationary) 또는 무버블 (Movable)이어야 한다.
- 소리나 포스트 프로세스 같은 추가 연출이 필요할 때만 블루프린트로 상속해 `BP_OnEnvironmentEffectChanged`를 쓴다.

### 6.5 몬스터 블루프린트 확인 (`BP_Golem_N/R/SR`)
- **Auto Possess AI**: `Placed in World or Spawned`
- **AI Controller Class**: `BP_AiController`
- 코드에서도 컨트롤러가 없으면 `SpawnDefaultController()`를 호출하지만, 위 설정이 되어 있어야 BT가 바로 돈다.

### 6.6 (선택) 아이템별 픽업 지정
- `DT_ItemStaticData`에 새 열 **Pickup Actor Class**가 생겼다. 아이템마다 떨어질 픽업 블루프린트를 지정할 수 있고, 비워두면 기본 픽업을 쓴다.

### 6.7 동작 확인
1. 플레이 → 단말기 버튼 문구가 자동 생성되는지 확인 (예: "골렘 2마리 소환").
2. 콘솔 `OecAddStack 5` → 페이즈 2 전환과 함께 열쇠가 떨어지는지 확인 (메인 문은 열쇠로 직접 연다, 9장).
3. 콘솔 `OecRunEffect Effect.Env.Darken 2` → 조명 변화 확인 → 버튼 2번 누르면 원래대로.
4. `OecShowState`로 상태 확인.

---

## 7. 아이템 투하와 소모품 사용 (2026-11-18)

### 7.1 투하 위치 바닥 보정
- 픽업 액터에는 물리가 없어서, 천장 투하구 위치에 그대로 생성하면 공중에 떠 있었다 (소방 도끼가 안 보이던 원인 중 하나).
- `AOecChoiceSpawnPoint::GetRandomGroundTransform`이 투하 위치에서 아래로 선을 쏴서 바닥(`WorldStatic`)을 찾고, `Effect.Give.Item`은 픽업의 바운드 아래쪽을 바닥에 맞춰 놓는다.
- 검사 거리는 스폰 지점 디테일 (Details) › **Ground Trace Distance** (기본 3000). 바닥을 못 찾으면 경고 로그를 남기고 투하구 위치에 놓는다.

### 7.2 소모품 사용 (`UOecGA_UseItem`)
| 이전 | 지금 |
|---|---|
| 어빌리티 블루프린트 하나가 데이터 행 하나에 고정 (`GA_Heal10` → 항상 같은 회복량) | 모든 소모품이 어빌리티 하나를 공유. 퀵슬롯 사용 시 `Event.Item.Use` 이벤트에 **아이템 코드를 실어** 보낸다 |
| 입력 ID(ItemUse)로 발동, 어빌리티가 없어도 아이템이 소모됨 | 이벤트로 발동, **어빌리티가 실제로 발동했을 때만** 아이템 소모 |
| GE와 회복량을 블루프린트마다 지정 | 아이템 데이터의 `HealthRestoreAmount`, `SanityRestoreAmount`, `UseEffects`를 선택지 효과 실행기로 적용 (GE: 프로젝트 설정의 Choice Health/Sanity Effect) |

- 새 소모품 = 데이터 테이블 행 추가. 어빌리티를 새로 만들지 않는다.
- `UseEffects`는 선택지 효과와 같은 구조라서, 이후 이벤트 아이템(질문 변경권 등)이나 버프 아이템도 같은 방식으로 붙일 수 있다.
- 플레이어 `DefaultAbilities`에 `UOecGA_UseItem`(또는 그 하위 블루프린트, 예: 기존 `GA_Heal10`)이 하나 들어 있어야 한다. 없으면 "소모품 사용 실패" 경고가 뜨고 아이템은 소모되지 않는다.
  - `GA_Heal10`에 있던 Item Data Row, Item Effect Class 속성은 삭제됐다 (더 이상 쓰지 않음).

### 7.3 에디터 작업 (사용자)
**`DT_ItemStaticData` 행 추가·수정**

| 행 이름 | Item Name | Item Type | Health Restore Amount | Pickup Actor Class |
|---|---|---|---|---|
| `Weapon_FireAxe` (기존) | - | - | - | `BP_Pickup_Axe` 지정 |
| `Heal_S` | 응급 붕대 | 소모품 (Consumable) | 10 | 회복 아이템 픽업 (없으면 비워 두기 = 기본 픽업) |
| `Heal_M` | 회복 주사 | 소모품 (Consumable) | 25 | 위와 같음 |
| `Heal_L` | 의료 키트 | 소모품 (Consumable) | 50 | 위와 같음 |

- 행 이름과 Item Code를 같게 쓴다 (다르면 시작할 때 오류 로그가 뜬다).

**`DT_MonsterStaticData` 행 추가** (좀비 3종, 기존 골렘 행을 복제해서 고치면 빠르다)

| 행 이름 | Monster Name | Monster Class | 위협도 (참고) |
|---|---|---|---|
| `Zombie_Walker` | 걷는 좀비 | `BP_ZombieMen` | 1 |
| `Zombie_Runner` | 달리는 좀비 | `BP_ZombieGirl` | 2 |
| `Zombie_Brute` | 거대 좀비 | `BP_SpecialZombie` | 3 |

- 체력·공격력·이동 속도·공격 범위·공격 몽타주·기본 어빌리티는 행마다 채운다. 위협도가 높을수록 강하게 (걷는 좀비 < 골렘 N ≈ 달리는 좀비 < 거대 좀비 ≈ 골렘 R).
- 각 좀비 블루프린트의 디테일 (Details) › OEC | Monster › **Monster Code**를 위 행 이름과 같게 넣는다.
- AI 컨트롤러 클래스 (AI Controller Class)는 `BP_AiController`, 자동 빙의 AI (Auto Possess AI)는 `Placed in World or Spawned` (6.5와 같음).

**질문 테이블 다시 임포트**: `ProjectData/QuestionStaticData_All.json` (좀비·회복 아이템 질문이 켜졌다).

### 7.4 동작 확인
1. 튜토리얼 2번에서 소방 도끼 선택 → 투하구 아래 바닥에 도끼 픽업이 놓이는지.
2. 튜토리얼 3번 → 걷는 좀비가 소환되고, 파란 쪽을 고르면 응급 붕대가 떨어지는지.
3. 응급 붕대를 줍고 퀵슬롯에 넣어 사용 → 체력이 10 오르고 개수가 1 줄어드는지 (`OecShowState`로 확인).
4. 콘솔 `OecSpawnMonster Zombie_Brute` → 거대 좀비 소환 확인.

---

## 8. 버프·답변 패시브 (2026-11-18)

### 8.1 구조
| 효과 | 구현 | 반영 위치 |
|---|---|---|
| 이동 속도 증가·감소 | GE가 `Speed`, `SprintSpeed` 어트리뷰트를 곱연산으로 변경 | 플레이어가 매 프레임 이동 속도에 반영 (기존 코드) |
| 점프력 증가 | GE가 `JumpForce` 변경 | 위와 같음 |
| 공격 속도 증가 | 신규 어트리뷰트 `AttackSpeed` (기본 1) | 원거리 연사 간격 ÷ 배율, 근접 공격·재장전 몽타주 재생 속도 × 배율 (`UOecPlayerAttributeSet::GetAttackSpeedMultiplier`) |
| 답변마다 체력·정신력 | 공용 베이스 `UOecChoiceEffect_AnswerStat` + 하위 2개. 지속 효과(AnswerModifier)로 걸고 답변마다 체력·정신력 실행기를 호출 | 다시 얻으면 변화량은 더해지고 남은 횟수는 갱신 |

- 버프·디버프는 **답변 횟수**로 유지한다. 시간제면 기다리기만 해도 디버프가 풀리기 때문이다.
  - 버프 GE는 무한 지속(Infinite)으로 만들고, 실행기가 지속 효과(AnswerModifier)로 남은 답변 수를 세다가 0이 되면 GE를 제거한다.
  - 지속 답변 수는 질문 데이터의 `Value` (0 이하면 설정 **Default Buff Answer Duration**, 기본 3).
  - 같은 버프를 다시 얻으면 GE를 겹쳐 걸지 않고 남은 답변 수만 갱신한다. 지속 효과는 태그 + 버프 ID(`Param`)로 구분해서, 서로 다른 버프는 각자 센다.
- 수치(배율)는 **GE에서** 정한다 (데이터).
- 버튼 문구: 버프는 프로젝트 설정 **Choice Buff Display Names**의 이름(기본값 4개 등록됨), 패시브는 "답변 10번 동안 매번 체력 +2".
- 공격 속도는 원거리 무기의 연사를 **시작할 때** 간격을 정하므로, 연사 중에 버프가 걸리면 다음 발사 입력부터 반영된다.

### 8.2 에디터 작업 (사용자)
**GE 4개 만들기** (`Content/01_OEC/07_GAS/GE/`, 블루프린트 클래스 → 부모 `GameplayEffect`)

모두 지속 시간 정책 (Duration Policy) = **무한 (Infinite)**. 끝나는 시점은 코드가 답변 횟수로 정한다.

| 에셋 | 모디파이어 (Modifiers) |
|---|---|
| `GE_Buff_Haste` | `OecAttributeSetBase.Speed` 곱하기 1.3, `OecPlayerAttributeSet.SprintSpeed` 곱하기 1.3 |
| `GE_Buff_Jump` | `OecPlayerAttributeSet.JumpForce` 곱하기 2.0 |
| `GE_Buff_AttackSpeed` | `OecPlayerAttributeSet.AttackSpeed` 곱하기 1.3 |
| `GE_Buff_Slow` | `OecAttributeSetBase.Speed` 곱하기 0.7, `OecPlayerAttributeSet.SprintSpeed` 곱하기 0.7 |

- 모디파이어 연산 (Modifier Op)은 곱하기 계열(UE 5.6 목록에서는 `Multiply Additive` 또는 `Multiply Compound`)을 고르고, 값은 **배율 그대로**(1.3 = 30% 증가, 0.7 = 30% 감소) 넣는다. 두 연산은 같은 버프가 여러 개 겹칠 때만 결과가 다르다 (Additive: 증가분끼리 더함, Compound: 배율끼리 곱함). 이름이 다르게 보이면 디테일 패널 검색창에 `Modifier Op`를 입력해 확인한다.
- 스태킹 (Stacking) 설정은 필요 없다. 같은 버프를 다시 받으면 코드가 GE를 다시 걸지 않고 남은 답변 수만 갱신한다.

**프로젝트 설정 › OEC Game Data Settings › Choice › Choice Buff Effects**에 등록

| 키 | 값 |
|---|---|
| `Buff_Haste` | `GE_Buff_Haste` |
| `Buff_Jump` | `GE_Buff_Jump` |
| `Buff_AttackSpeed` | `GE_Buff_AttackSpeed` |
| `Buff_Slow` | `GE_Buff_Slow` |

**질문 테이블 다시 임포트**: `ProjectData/QuestionStaticData_All.json` (버프·패시브 질문 15개가 켜졌다).

### 8.3 동작 확인
1. 콘솔 `OecRunEffect Effect.Buff 3 Buff_Haste` → 걷기·달리기가 빨라지는지, 질문에 3번 답한 뒤 돌아오는지 (기다리기만 해서는 풀리지 않음).
2. `OecRunEffect Effect.Buff 3 Buff_AttackSpeed` → 라이플 연사, 도끼 휘두르기, 재장전이 빨라지는지. `OecShowState`에 남은 답변 수가 표시되는지.
3. `OecRunEffect Effect.Passive.AnswerHealth 2 10` → 질문에 답할 때마다 체력 +2, `OecShowState`에 지속 효과 표시, 10번 뒤 사라지는지.

---

## 9. 열쇠와 문, 아이템 낙하 (2026-11-18)

### 9.1 바뀐 규칙
| 이전 | 지금 |
|---|---|
| 페이즈 2 스택 = 열쇠 **스택** (숫자, 시작 3개) | 페이즈 2 "스택" 보상 = **열쇠 아이템**(`Key_Door`)이 투하구에서 떨어짐. 버튼 문구 "열쇠 1개 획득" |
| 전구 5개 → 메인 문 자동 개방 | 전구 5개 → 페이즈 2 전환 + **열쇠 1개 투하**. 메인 문은 열쇠를 들고 가서 직접 연다 |
| 랜덤 방 문: 열쇠 스택 소모 | 모든 문: 인벤토리의 열쇠 `RequiredKeyCount`개(기본 1) 소모 |
| 투하된 아이템이 바닥에 바로 놓임 | 투하구에서 **떨어져서** 바닥에 닿으면 착지 소리 |

- 판단은 한 곳(`UOecStageManagerSubsystem::AddStack`)에서 한다: 페이즈 1이면 전구, 페이즈 2면 `GrantKeys` → 아이템 지급 효과. 스택 효과·이벤트 보상·"답변마다 스택 확률" 패시브가 모두 이 경로를 쓰므로 따로 고칠 곳이 없다.
- 스택 보정(천장)은 페이즈 2에서 열쇠 지급(`OnKeyRewarded`)을 스택 획득으로 센다.
- 열쇠 스택(`UOecEventSubsystem::ModifyStacks` 등)은 삭제했다. 블루프린트에서 쓰던 곳은 없었다.
- 낙하: 물리 없이 중력 가속(기본 1800cm/s², 픽업 디테일 › Drop Gravity)으로 떨어지고, 움직이는 동안만 Tick. 착지 연출은 `BP_OnLanded`.

### 9.2 에디터 작업 (사용자)
1. **`DT_ItemStaticData`에 열쇠 행 추가**: 행 이름 `Key_Door`, Item Name "열쇠", Item Type **퀘스트아이템 (KeyItem)**, 픽업 액터 클래스 (Pickup Actor Class)는 열쇠 모양 픽업 (없으면 비워 두기 = 기본 픽업).
   - 다른 코드를 쓰고 싶으면 프로젝트 설정 › Stage › **Key Item Code**를 바꾼다.
2. **착지 소리**: 아이템 행마다 새 칸 **Drop Sound**에 사운드를 지정한다. 비워 두면 프로젝트 설정 › Choice › **Default Item Drop Sound**를 쓴다 (공통 소리 하나만 넣어도 됨).
3. **문**: 문 블루프린트(메인 문, `BP_PhaseDoor1/2`, 랜덤 방 문)의 디테일 › OEC | Door에서
   - 예전 칸 `Required Stacks`, `Should Consume Stack`은 없어졌다. 새 칸 **Required Key Count** (기본 1)를 확인한다.
   - 열쇠가 없을 때의 거절 소리·흔들림은 이벤트 그래프의 `BP_OnOpenDenied`에 연결한다.
4. 페이즈 1 완료 열쇠 수는 프로젝트 설정 › Stage › **Phase1 Complete Key Count** (기본 1).

### 9.3 동작 확인
1. `OecAddStack 5` → 열쇠가 투하구에서 떨어지며 착지 소리 → 줍기 → 메인 문 앞 E → 열림.
2. 열쇠 없이 메인 문 E → 열리지 않고 로그 "열쇠 부족 (필요 1, 보유 0)".
3. 페이즈 2에서 `OecAddStack 1` → 열쇠가 하나 더 떨어짐 → 복도 방 문을 열쇠로 연다. `OecShowState`에 "열쇠 N개".
