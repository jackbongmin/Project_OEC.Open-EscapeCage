# 개발 로드맵

원칙: **기능을 먼저 완성하고, 질문지는 마지막에 한 번에 작성한다.** 그 전까지는 테스트용 질문 몇 개로 돌린다.

## ▶ 다음 작업 (2026-11-16 기준)
1. **에디터에서 방 어두워짐 확인**: `OecRoomEnvironmentActor`의 Darken Lights에 방 조명 지정 → `OecRunEffect Effect.Env.Darken 2`로 확인 ([ChoiceSystem.md 6.4](ChoiceSystem.md))
2. **상호작용 개편 확인**: 범위 콜리전 + 시선 선택 ([InteractionAndMelee.md 1.3](InteractionAndMelee.md))
3. **근접 무기 에디터 연결**: 검 블루프린트, GA, 데이터 테이블 행, 몽타주 노티파이 ([InteractionAndMelee.md 2.3](InteractionAndMelee.md))
4. **몬스터 사망 시 시야 꺼짐 확인**: `OecSpawnMonster Golem_N 1` → 사살 → 원뿔이 바로 사라지는지, 7초 뒤 반납 후 다시 소환했을 때 원뿔·추적이 정상인지 ([TS-003](../03_Troubleshooting/TS-003_몬스터_사망_후_시야_유지.md))
5. 데이터 구조체 초기화 에러 15건 정리 ([ImplementationStatus 버그 12](ImplementationStatus.md))

### M2 전구 에디터 연결
구조: `AOecStackIndicator`(스택 구독, 몇 칸 켤지 결정) → `IOecStackDisplayInterface` → `AOecStackBulb`(머티리얼 교체 + 포인트 라이트).
1. 콘텐츠 브라우저에서 `OecStackBulb`를 부모로 블루프린트 `BP_StackBulb` 생성 → `BulbMesh`의 스태틱 메시 (Static Mesh)에 `SM_Lamp_0X` 지정.
2. `BP_StackBulb` 디테일 (Details) 패널 `OEC|Stack|Material`
   - `Unlit Material`(꺼짐): `MI_Atlas_Grey`
   - `Lit Material`(켜짐): `MI_Atlas_Grey_E_01`
   - 위치: `Low_Poly_Sci-Fi_Corridor/Materials/Alternate/Grey/`
   - `Material Slot Index`: 바꿀 슬롯 번호 (기본 0, `-1`이면 모든 슬롯)
3. 메인 방에 `BP_StackBulb` 5개 배치 → 각 전구의 `Stack Order`에 **1~5** 지정. 뷰포트에 번호가 떠서 배치 순서를 바로 볼 수 있다 (미지정은 빨간 "순번 미지정", 게임에서는 안 보임).
   - 밝기·반경·색은 각 인스턴스의 `BulbLight`에서 조절. `Preview Lit In Editor`를 켜면 에디터에서 켜진 모습을 볼 수 있다.
4. `OecStackIndicator` 1개 배치. 기본값(`Auto Collect Displays` 켜짐)이면 맵의 전구를 `Stack Order` 순으로 자동으로 모으므로 따로 지정할 것이 없다. 순번 누락·중복은 출력 로그 (Output Log)에 경고로 나온다.
5. 확인: `OecAddStack 1`을 반복 → 한 칸씩 켜지고, 5개째에 페이즈 2 전환·메인 문 개방. 소리·깜빡임은 `BP_StackBulb`의 `BP_OnLitChanged`(In Play Effect가 true일 때)에서 추가.

---

## M0. 정리 — 완료
- [x] 스택 정리: 질문 결과를 현재 페이즈의 스택(전구/열쇠)으로 연결, 메인 문을 페이즈 전환에 연결 ([TS-002](../03_Troubleshooting/TS-002_페이즈_스택_연결_끊김.md))
- [x] 에디터: `BP_MainDoor` 생성 후 `DoorType`을 `MainDoor`로 설정
- [x] 체력 클램프 버그 (`Clamp(0, 100)` → `GetMaxHealth()`), 플레이어 체력 시작값 50 복구 ([TS-001](../03_Troubleshooting/TS-001_GAS_체력_클램프.md))
- [x] 네이티브 게임플레이 태그 파일 추가 (`GAS/OecGameplayTags.h`, ini 태그 10개 이전)
- [x] 아키텍처 원칙, 트러블슈팅 기록 규칙 문서화

## M1. 선택지 효과 시스템 (핵심) — 완료
상세: [ChoiceSystem.md](ChoiceSystem.md)
- [x] 선택지 → 결과(가중치) → 효과 목록 데이터 구조 (GDD 7.2)
- [x] 효과 실행기 10종: 없음, 스택, 체력, 정신력, 즉사, 버프(GE), 몬스터 소환, 아이템 지급, 지속 효과(답변당 스택 확률), 환경(어두워짐)
- [x] 메인 방 천장 스폰구·투하구 액터 (`AOecChoiceSpawnPoint` + `UOecSpawnPointSubsystem`)
- [x] 환경 반응 액터 (`AOecRoomEnvironmentActor`), 방 어두워짐은 C++로 조명 밝기 조절
- [x] 선택지 문구 자동 생성 (효과 데이터 → "체력 10 회복", 확률 결과 → "50% 확률로 ...")
- [x] 테스트용 질문 JSON 10개
- [x] 치트 커맨드 (`OecAddStack`, `OecSetPhase`, `OecSetHealth`, `OecSetSanity`, `OecSpawnMonster`, `OecGiveItem`, `OecRunEffect`, `OecShowState`)
- [x] 전구 개수를 프로젝트 설정으로 분리 (`Phase1MaxStack`)
- [x] 에디터: GE 2개 생성, 프로젝트 설정, 질문 테이블 재임포트, 스폰 지점 배치
- [x] 에디터에서 동작 확인 (사용자 확인, 2026-11-14)
- [ ] 방 어두워짐 C++ 버전 에디터 확인 (다음 작업 1)

## M2. 페이즈 1 완성 → 첫 플레이 테스트
- [x] 전구 점등 액터와 스택 연동 C++ (`AOecStackIndicator`, `AOecStackBulb`, `IOecStackDisplayInterface`)
- [x] 전구 에디터 배치와 동작 확인 (사용자 확인, 2026-11-16)
- [x] 상호작용 개편 C++: 범위 콜리전 후보 + 시선 선택 ([InteractionAndMelee.md](InteractionAndMelee.md)) — 에디터 확인 대기
- [x] 몬스터 사망·풀 반납 시 AI 시야·BT 정지, 풀링 연결 정리 ([TS-003](../03_Troubleshooting/TS-003_몬스터_사망_후_시야_유지.md)) — 에디터 확인 대기
- [ ] 스택 보정 (N번 연속 실패 시 스택 선택지 보장)
- [ ] 5개 점등 → 메인 방 문 개방 → "축하합니다, 탈출입니다" → 복도 공개 → 조롱 문구 연출
- [ ] 정신력 감소 (답변 -2, 피격 30당 -5)
- [ ] 플레이어 사망 처리

## M3. 페이즈 2
- [ ] 단말기 미니맵에서 방 선택 → 스택 소모 → 문 개방
- [ ] 에어락 방 → 복도 2 이동
- [ ] 보급 방, 위험 방 내용 랜덤화
- [ ] 정신력 5단계 효과 (GE), 패닉 시 선택지 글자 깨짐
- [ ] 미닫이 문 열림 연출 공통화 (`BP_PhaseDoor1/2`, `BP_MainDoor`의 중복 그래프 정리)

## M4. 엔딩과 연출
- [ ] 노말·진·사망·붕괴 엔딩
- [ ] WARDEN 대사, 글리치·픽셀 포스트 프로세스
- [ ] 단말기 타자 효과: 질문·선택지가 한 글자씩 "삐빅" 소리와 함께 출력, 출력 중 버튼 잠금, E로 빨리 넘기기, 저정신력 시 글자 깨짐 (구현 가능 확인, 2026-11-16)
- [x] 근접 무기 C++ (`AOecMeleeWeapon`, `UOecGA_MeleeAttack`, 카메라 스윕 판정) — 에디터 연결 대기 ([InteractionAndMelee.md](InteractionAndMelee.md))
- [ ] 근접 콤보, 막기

## M5. 질문지 일괄 작성 + 밸런싱
기준서: [QuestionGuideline.md](../01_Design/QuestionGuideline.md)
- [x] 질문 가이드라인 v0.2 (구성 207개, 몬스터·아이템·이벤트 목록, 가치 환산표, 스택 시뮬레이션, 스토리 6갈래, Fable 작업 지시서)
- [x] 주인공 설정 확정: 아이를 해친 죄수, 처벌 실험 시뮬레이션에서 반복
- [ ] Fable로 질문지 4배치 작성 + 밸런스 리포트 (가이드라인 13장)
- [ ] 출제 기능 확장 (선행 조건, 페이즈 제한, 고정 순서, 연속 제한, 신규 타입, 스토리 성향 기록)
- [ ] 이벤트 시스템 (`Effect.Event.Start`, `DT_RoomEvent`, 이벤트 12종), 이벤트 아이템 5종
- [ ] 신규 콘텐츠: 좀비 4종·로봇·외계 생명체·철갑 괴물, 회복템 3단계, 칼·최상급 근접 무기
- [ ] 무의미(밈) 질문, 스토리 질문 작성
- [ ] 효과 풀 정리, 등장 가중치 조정
- [ ] 최적화 백로그 처리 ([OptimizationBacklog.md](OptimizationBacklog.md))
