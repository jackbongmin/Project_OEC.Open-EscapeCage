# 구현 현황 (기획 대비)

기준: `develop` 브랜치, 마지막 커밋 `1c8d27d <feat> Monster Attack, Die 구현` (2026-04-13)
코드 규모: C++ 약 6,000줄 (`Source/OEC`)

상태 표기: ✅ 완료 · ⚠️ 일부 · ❌ 미구현

## 1. 기획 항목별 상태

| GDD 항목 | 상태 | 관련 코드 | 비고 |
|---|---|---|---|
| 1인칭 이동, 달리기, 점프, 스태미나 | ✅ | `OecPlayerCharacter` | |
| E 상호작용 | ✅ | `OecInteractionComponent`, `OecInteractableBase` | |
| 단말기 질문 표시 (가중치 랜덤, 중복 방지) | ✅ | `OecTerminalBase`, `OecTerminalWidget` | |
| 빨강/파랑 물리 버튼 | ✅ | `OecQuestionButton` | |
| 질문 결과 → 실제 효과 | ⚠️ | `OecEventSubsystem` | `Event.Stack.Add`만 처리. 스탯·스폰·아이템 핸들러는 TODO |
| 스택(전구) | ⚠️ | `OecEventSubsystem`, `OecStageManagerSubsystem` | **두 곳에 따로 존재** (아래 버그 1) |
| 페이즈 전환 | ⚠️ | `OecStageManagerSubsystem::ChangePhase` | 델리게이트만 있고 실제 연출·문 개방 연결 없음 (블루프린트 미확인) |
| 문 (스택 소모 개방) | ✅ | `OecDoorBase` | |
| 복도 방 랜덤 배치 | ✅ | `GeneratePhase2Rooms` | 문 8개 위치에 방 레벨 인스턴스 스트리밍 |
| 에어락 → 복도 2 이동 | ❌ | | GDD 5.4 신규 |
| 미니맵에서 방 선택 | ❌ | | |
| 원거리 무기 (권총, 라이플 N/R/SR) | ✅ | `OecRangedWeapon`, `OecGA_WeaponFire`, `OecGA_WeaponReload` | 반동, 줌, VFX 포함 |
| 근접 무기 | ❌ | `OecMeleeWeapon` | 함수 껍데기만 있음 |
| 샷건 | ❌ | | |
| 인벤토리 20칸, 퀵슬롯 5칸 | ✅ | `OecInventorySubsystem`, `OecQuickSlotSubsystem` | 아이템 설명창 TODO |
| 소모품 사용 (체력 회복) | ✅ | `OecGA_UseItem` | 정신력·이속 아이템 없음 |
| 몬스터 AI (시야, BT, 공격, 사망) | ✅ | `OecAIController`, `OecMonsterBase`, `OecGA_MonsterAttack` | 골렘 N/R/SR |
| 풀링 | ✅ | `OecPoolingSubsystem` | 몬스터에 적용 |
| 정신력 수치와 UI 바 | ⚠️ | `OecPlayerAttributeSet`, `OecStatPanelWidget` | 감소·단계 효과 로직 없음 |
| 정신력 5단계 효과 | ❌ | `FSanityStaticData` | 구조체만 존재 |
| AnswerCount | ❌ | | TODO |
| 플레이어 사망 | ❌ | | |
| 엔딩 | ❌ | | |
| WARDEN 대사, 연출 | ❌ | | |
| 글리치/픽셀 포스트 프로세스 | ❌ | | |

## 2. 알려진 버그와 기술 부채

1. **스택이 두 개다**
   - `UOecStageManagerSubsystem::CurrentStack` (페이즈 전환 판정)
   - `UOecEventSubsystem::PlayerStacks` (버튼, 문이 사용, 초기값 3)
   - 버튼으로 스택을 쌓아도 `AddStack`이 호출되지 않아 페이즈 2로 넘어가지 않는다. → 하나로 통일.
2. **체력 상한 하드코딩**: `OecAttributeSetBase.cpp:25`에서 `Clamp(Health, 0, 100)`. MaxHealth가 100보다 큰 몬스터는 체력이 깎여 시작한다. → `GetMaxHealth()`로 클램프.
3. **AttackPower 의미 불일치**: MMC는 퍼센트 보너스, 몬스터 공격은 고정 피해로 사용 ([DesignReview 2.13](../01_Design/DesignReview.md)).
4. **플레이어 체력 시작값 50**: `OecPlayerCharacter.cpp:67`. 회복 아이템 테스트용으로 보임 → 정식 값으로 복구.
5. **`MonsterBase::BeginPlay`의 중복 초기화**: 체력 100 설정 후 데이터로 다시 덮어쓰고, else 분기에서 또 100 설정.
6. **Tick 남용**: 단말기는 Tick이 필요 없는데 켜져 있다. AI 컨트롤러는 매 프레임 디버그 원뿔을 그린다.
7. **게임플레이 태그 문자열 조회**: `RequestGameplayTag(FName("..."))`를 발사·공격 같은 자주 호출되는 코드에서 매번 호출 → 네이티브 태그로 교체.
8. **데이터 테이블 하드 레퍼런스**: `FItemStaticData`의 `UTexture2D*`, `UAnimMontage*`, `FMonsterStaticData`의 몽타주가 하드 레퍼런스라, 데이터 테이블을 로드하는 순간 모든 아이콘과 몽타주가 메모리에 올라간다.
9. **로그 과다**: `LogTemp` + 이모지 로그가 발사·피격마다 출력된다. 전용 로그 카테고리로 분리 필요.
10. **`OEC_TestClass`**: 사용하지 않는 테스트 클래스.
11. **Git LFS 미사용**: `.uasset`/`.umap`을 일반 git으로 관리 중. `.git` 폴더가 이미 1.3GB.

## 3. 블루프린트 쪽 확인이 필요한 것
`.uasset`은 바이너리라 코드 분석으로 확인하지 못했다. 에디터에서 직접 확인 필요.
- `BP_PhaseDoor1/2`, `BP_RandomRoom`, `BP_EscapeBtn`에서 `AddStack`, `GeneratePhase2Rooms`를 호출하는지
- `GM_OEC`, `OECHUD`의 구성
- `BT_Monster`의 노드 구성
