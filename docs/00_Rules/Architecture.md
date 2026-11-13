# 아키텍처 원칙

목표: **실무 프로젝트 수준의 구조와 확장성.** 포트폴리오에서 "왜 이렇게 설계했는지" 설명할 수 있는 코드를 만든다.
새 클래스를 만들 때는 이 문서를 따른다. 코드 작성 양식은 [CodingConvention.md](CodingConvention.md).

---

## 1. 폴더 구조 (`Source/OEC/Public`, `Private` 동일)

| 폴더 | 역할 | 예 |
|---|---|---|
| `Core/` | 게임모드, 플레이어 컨트롤러, HUD | `AOecGameModeBase` |
| `Settings/` | 프로젝트 설정 (`UDeveloperSettings`) | `UOecGameDataSettings` |
| `Data/` | 데이터 구조체, enum | `FItemStaticData`, `EDoorType` |
| `Subsystems/` | 전역 상태와 시스템 (게임 인스턴스 / 월드) | `UOecInventorySubsystem` |
| `GAS/` | 어빌리티, 이펙트, 어트리뷰트, 태그 | `UOecGA_WeaponFire`, `OecGameplayTags` |
| `Choice/` | 선택지 효과 시스템 (서브시스템 + `Effects/` 실행기) | `UOecChoiceSubsystem`, `UOecChoiceEffect_Stack` |
| `Actors/` | 월드에 배치되는 액터 (기능별 하위 폴더) | `Actors/Interactable/`, `Actors/Item/Weapon/`, `Actors/Room/` |
| `Components/` | 액터에 붙이는 재사용 기능 | `UOecInteractionComponent` |
| `Interfaces/` | 클래스 간 계약 | `IOecInteractableInterface`, `IOecPoolableInterface` |
| `AI/` | AI 컨트롤러, BT 태스크·서비스·데코레이터 | `UOecBTTask_UseAbility` |
| `Anim/` | 애님 인스턴스, 애님 노티파이 | |
| `UI/` | 위젯 (`UI/Ingame/`, 이후 `UI/Menu/` 등) | |
| `System/` | 위 분류에 속하지 않는 지원 클래스 | 드래그앤드롭 오퍼레이션 |

새 기능이 여러 파일로 커지면 하위 폴더를 만든다 (예: `Subsystems/Choice/`, `GAS/Effect/`).

---

## 2. 계층과 의존 방향

```
Data / Settings          ← 순수 데이터. 다른 계층을 모름
   ↑
Subsystems / GAS         ← 규칙과 상태. 데이터를 읽고 이벤트(델리게이트)를 발행
   ↑
Actors / Components      ← 월드 표현. 서브시스템에 요청하고 이벤트를 구독
   ↑
UI                       ← 표시만. 델리게이트를 구독해 갱신
```

- 위 계층이 아래 계층을 쓰고, **아래 계층은 위 계층을 직접 참조하지 않는다.**
  - 예: 서브시스템이 특정 액터 클래스를 직접 찾아 조작하지 않는다. 델리게이트를 발행하면 액터가 구독한다. (`OnPhaseChanged` → `AOecDoorBase`)
- UI는 Tick에서 값을 읽지 않는다. 델리게이트나 GAS 어트리뷰트 변경 이벤트로만 갱신한다.

---

## 3. 설계 원칙

### 3.1 데이터 주도
- 밸런스 수치, 콘텐츠 목록은 코드나 블루프린트에 직접 쓰지 않는다. **DataTable / DataAsset**으로 뺀다.
- 테이블은 `UOecGameDataSettings`에 등록하고 `UOecGameDataSubsystem`에서 캐싱해 조회한다. 새 테이블도 같은 경로를 따른다.
- 데이터 구조체의 에셋 참조(텍스처, 몽타주, 클래스)는 `TSoftObjectPtr` / `TSoftClassPtr`로 둔다. 필요할 때 로드한다.
- 텍스트로 관리할 원본 데이터는 `ProjectData/`에 CSV/JSON으로 두고 임포트한다.

### 3.2 책임 분리
- 클래스 하나는 한 가지 일만 한다. 서브시스템도 책임별로 나눈다 (인벤토리 / 퀵슬롯 / 스테이지 / 이벤트처럼).
- 하나의 서브시스템이 커지면, 기능 단위 클래스(UObject)로 쪼개고 서브시스템은 조립만 한다.

### 3.3 확장 지점은 "추가"로 늘린다
- 새 종류를 추가할 때 기존 코드의 `switch`/`if` 사슬을 고치지 않도록 만든다.
  - 예 (M1 선택지 효과): 효과마다 실행 클래스를 두고, 태그 → 실행 클래스를 등록하는 방식. 새 효과는 클래스 하나 추가로 끝난다.
- 상속은 얕게 (베이스 → 구체 클래스 2단 정도). 공통 기능은 컴포넌트나 인터페이스로 공유한다.

### 3.4 GAS 활용 규칙
- 스탯은 `AttributeSet`, 스탯 변화는 `GameplayEffect`, 행동은 `GameplayAbility`.
- 액터가 어트리뷰트 값을 직접 `Set`하는 것은 초기화에만 쓴다. 게임 중 변화는 GE로 한다.
- 태그는 [OecGameplayTags.h](../../Source/OEC/Public/GAS/OecGameplayTags.h)의 네이티브 태그만 쓴다. 문자열 조회(`RequestGameplayTag`) 금지.

### 3.5 성능 기본값
- Tick은 기본으로 끈다. 필요할 때만 켜고, 주기가 길어도 되면 타이머를 쓴다.
- 자주 생성·파괴되는 액터는 `UOecPoolingSubsystem`을 쓴다.

### 3.6 블루프린트와의 경계
- C++: 규칙, 상태, 계산, 데이터 처리.
- 블루프린트: 연출(애니메이션, 사운드, 이펙트), 레이아웃, 에셋 지정.
- C++은 `BlueprintImplementableEvent`(`BP_On...`)로 연출 지점을 열어 둔다.

---

## 4. 새 클래스 체크리스트
- [ ] 폴더 위치가 위 표에 맞는가
- [ ] 수치와 에셋 참조가 데이터로 빠져 있는가 (소프트 레퍼런스)
- [ ] 아래 계층이 위 계층을 참조하지 않는가 (델리게이트로 통신)
- [ ] 새 종류 추가 시 기존 코드를 고치지 않아도 되는가
- [ ] Tick이 꼭 필요한가
- [ ] 태그는 네이티브 태그를 썼는가
- [ ] [CodingConvention.md](CodingConvention.md) 양식을 따랐는가
