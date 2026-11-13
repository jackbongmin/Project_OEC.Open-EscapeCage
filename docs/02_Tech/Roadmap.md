# 개발 로드맵

원칙: **기능을 먼저 완성하고, 질문지는 마지막에 한 번에 작성한다.** 그 전까지는 테스트용 질문 몇 개로 돌린다.

## ▶ 다음 작업 (2026-11-14 기준)
1. **에디터 다시 빌드 후 방 어두워짐 확인**: `OecRoomEnvironmentActor`의 Darken Lights에 방 조명 지정 → `OecRunEffect Effect.Env.Darken 2`로 확인 ([ChoiceSystem.md 6.4](ChoiceSystem.md))
2. **전구 점등 액터 (M2 첫 작업)**: `StageManager::OnStackChanged`를 구독해 스택 수만큼 전구를 켜는 C++ 액터. 맵에서 전구 5개를 순서대로 지정만 하면 되게 만든다.
   - 시작 전 확인할 것: 메인 화면 옆 전구가 **라이트 / 스태틱 메시 / UI 위젯** 중 무엇인지
3. 데이터 구조체 초기화 에러 15건 정리 ([ImplementationStatus 버그 12](ImplementationStatus.md))

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
- [ ] 전구 5개 점등 액터와 스택 연동 (다음 작업 2)
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
- [ ] 근접 무기

## M5. 질문지 일괄 작성 + 밸런싱
- [ ] 무의미(밈) 질문, 스토리 질문 작성
- [ ] 효과 풀 정리, 등장 가중치 조정
- [ ] 최적화 백로그 처리 ([OptimizationBacklog.md](OptimizationBacklog.md))
