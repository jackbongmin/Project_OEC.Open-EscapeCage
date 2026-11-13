# C++ 코딩 컨벤션

기존 코드(`Source/OEC`)에서 정리한 작성 규칙. 새 코드나 수정 코드는 이 규칙을 따른다.

## 1. 들여쓰기와 중괄호
- 들여쓰기는 **탭**. 최근 작성한 파일(몬스터, AI, GA, 인벤토리, UI)이 모두 탭이다.
  - 예전 파일 중 4칸 공백인 파일(`OecPlayerCharacter`, `OecDoorBase`, `OecTerminalBase` 등)을 수정할 때는 **그 파일의 기존 들여쓰기를 따른다.** 한 파일 안에서 섞지 않는다.
- 중괄호는 항상 다음 줄에 (Allman 스타일).
- 한 줄 조기 반환은 중괄호 없이: `if (!eventSubsystem) return;`

## 2. 이름 규칙
| 대상 | 규칙 | 예 |
|---|---|---|
| 클래스 | UE 접두사 + `Oec` | `AOecDoorBase`, `UOecEventSubsystem`, `UOecGA_WeaponFire` |
| 함수 | PascalCase | `ModifyStacks`, `SetDoorState` |
| 델리게이트 콜백 | `On` + 이벤트 | `OnHealthChanged`, `OnTargetDetected`, `OnMontageCompleted` |
| 내부 처리 함수 | `Handle` + 대상 | `HandleSpawnMonster`, `HandleGiveItem` |
| 블루프린트 구현 이벤트 | `BP_On` + 이벤트 | `BP_OnDoorStateChanged` |
| 멤버 변수 | PascalCase | `CurrentStack`, `RequiredStacks` |
| bool 멤버 | `b` 접두사 | `bIsOpen`, `bShouldConsumeStack` |
| 함수 매개변수 | `In` 접두사 | `InAmount`, `InNewPhase`, `InInteractor` |
| bool 매개변수 | `bIn` 접두사 | `bInOpen`, `bInLocked` |
| 출력 매개변수 | `Out` 접두사 | `OutCacheMap` |
| 지역 변수 | lowerCamelCase | `eventSubsystem`, `currentStack`, `monsterData`, `asc` |
| 델리게이트 타입 | `FOn...Delegate` | `FOnStackChangedDelegate`, `FOnButtonClickedDelegate` |
| enum | `E` 접두사, `UMETA(DisplayName = "한국어")` | `EDoorType::MainDoor` |

## 3. 헤더 구성
- 맨 위 주석 `// Fill out your copyright notice ...` 유지.
- 전방 선언(`class UDataTable;`)을 `UCLASS` 위에 둔다.
- 섹션 순서 (필요할 때마다 `public:` / `protected:` 블록을 다시 열어 기능별로 묶는다)
  1. `public:` 생성자, 공개 함수
  2. `protected:` `virtual void BeginPlay() override;` 등 오버라이드, 내부 함수
  3. `protected:` `UPROPERTY` 멤버
  4. `public:` 블루프린트용 Getter, 델리게이트
- Getter는 헤더에 한 줄 인라인: `FName GetMonsterCode() const { return MonsterCode; }`
- `UFUNCTION` / `UPROPERTY` 카테고리는 `"OEC|분류"` 형식: `"OEC|Door"`, `"OEC|Stage"`, `"OEC|Events"`.
- 속성 위에 필요할 때만 한국어 한 줄 주석.

## 4. 소스 구성
- include는 자기 헤더 → 관련 헤더 순, 성격이 다른 묶음 사이는 빈 줄.
- 서브시스템 접근
  ```cpp
  UOecEventSubsystem* eventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>();
  if (!eventSubsystem) return;
  ```
  또는 if-초기화: `if (UOecStageManagerSubsystem* stageManager = ...)`
- 캐스트도 if-초기화를 즐겨 쓴다: `if (AOecPlayerCharacter* player = Cast<AOecPlayerCharacter>(Actor))`
- float 리터럴은 `f` 접미사: `100.0f`, `0.f`

## 5. 주석과 로그
- 주석은 한국어, 짧게. 단계 설명이 필요하면 `// 1. ...`, `// 2. ...` 번호를 붙인다.
- 로그는 `UE_LOG(LogTemp, ...)` + 한국어 메시지.
  - 액터 로그는 이름을 앞에: `TEXT("[%s] 개방 성공"), *GetName()`
  - 실패: `Error`, 경고·조건 불충족: `Warning`, 일반 흐름: `Log`
- 이후 로그 카테고리를 분리하면 ([OptimizationBacklog](../02_Tech/OptimizationBacklog.md)) 이 항목을 갱신한다.
