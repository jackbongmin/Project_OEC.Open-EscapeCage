# 최적화 및 개발 효율 백로그

우선순위: 🔴 높음 (지금 해두면 나중 비용이 크게 줄어듦) · 🟡 중간 · 🟢 낮음 (출시 전 다듬기)

## 1. 프로젝트 관리

| 우선 | 항목 | 내용 |
|---|---|---|
| 🔴 | Git LFS 도입 | `.uasset`, `.umap`, 텍스처, 사운드를 LFS로 관리. 현재 `.git`이 1.3GB로, 에셋을 수정할 때마다 저장소가 계속 커진다. 기존 히스토리 마이그레이션(`git lfs migrate`)은 히스토리를 다시 쓰므로 별도 결정 필요 |
| 🟡 | 미사용 에셋 정리 | 데모 맵, 마켓플레이스 샘플 콘텐츠(`CombatMagicAnims/Demo`, `Stone_Golem/demo` 등) 제거 → 용량, 쿠킹 시간 감소 |
| 🟢 | `OEC_TestClass` 등 테스트 코드 제거 | |

## 2. 메모리와 로딩

| 우선 | 항목 | 내용 |
|---|---|---|
| 🔴 | 데이터 테이블 소프트 레퍼런스화 | `UTexture2D*`, `UAnimMontage*` → `TSoftObjectPtr`. 필요할 때 비동기 로드 |
| 🔴 | 방 레벨 미리 로드 | 페이즈 1 진행 중에 복도 1 방 레벨 인스턴스를 비동기로 미리 스트리밍해 둔다. 페이즈 전환 순간 끊김 방지 |
| 🟡 | 방 언로드 | 방문이 끝난 위험 방(몬스터 전멸)은 일정 거리 이상 멀어지면 언로드 |
| 🟡 | 비동기 무기 로드 | `SetWeaponState`의 `LoadSynchronous` → `FStreamableManager::RequestAsyncLoad` (또는 아이템을 주울 때 미리 로드) |
| 🟢 | Asset Manager / Primary Asset 등록 | 아이템, 몬스터를 Primary Asset으로 관리하면 쿠킹·청크 분리가 쉬워짐 |

## 3. CPU (게임 스레드)

| 우선 | 항목 | 내용 |
|---|---|---|
| 🔴 | 네이티브 게임플레이 태그 | `UE_DEFINE_GAMEPLAY_TAG`로 태그를 한 곳에 선언. 문자열 조회 제거 + 오타 방지 |
| 🔴 | 불필요한 Tick 끄기 | 단말기, 상호작용 컴포넌트(타이머 0.1초 간격 트레이스로 충분) |
| 🟡 | 이벤트 기반 UI | 스탯 UI를 ASC 어트리뷰트 변경 델리게이트로만 갱신 (매 프레임 갱신 금지) |
| 🟡 | AI 비용 관리 | 퍼셉션 업데이트 간격 조정, 플레이어가 없는 방의 몬스터는 BT 정지, 디버그 원뿔은 콘솔 변수로 켜고 끄기 |
| 🟡 | 애니메이션 최적화 | 몬스터 `VisibilityBasedAnimTickOption = OnlyTickPoseWhenRendered`, URO(Update Rate Optimization) 사용 |
| 🟡 | 풀링 확대 | 탄 트레이서, 임팩트 VFX는 Niagara 자체 풀(`ENCPoolMethod::AutoRelease`), 드롭 아이템, 데칼도 풀링 |
| 🟢 | 로그 카테고리 분리 | `DECLARE_LOG_CATEGORY_EXTERN(LogOec...)`. 쉬핑 빌드에서 상세 로그 제거 |

## 4. 렌더링 (GPU)

| 우선 | 항목 | 내용 |
|---|---|---|
| 🟡 | 닫힌 문 = 컬링 | 방 레벨은 문이 열릴 때 스트리밍하면 자연스럽게 렌더링 대상이 줄어든다 |
| 🟡 | 그림자 드리우는 동적 광원 수 제한 | 방마다 그림자 광원 1~2개 이하 |
| 🟡 | Lumen / 스케일러빌리티 설정 | 좁은 실내 맵이라 Lumen 품질을 낮춰도 티가 적다. 저사양 옵션에서 Lumen 끄기 |
| 🟢 | 포스트 프로세스 비용 관리 | 글리치·픽셀화 효과는 머티리얼 하나로 합치고, 정신력 단계별로 파라미터만 변경 |
| 🟢 | 대용량 스카이박스 텍스처 | `TC_Skybox_05`(git 제외됨) 해상도·압축 확인 |

## 5. 개발 효율 (디버그 도구)

| 우선 | 항목 | 내용 |
|---|---|---|
| 🔴 | 치트 커맨드 | `CheatManager`로 `Oec.AddStack 5`, `Oec.SetSanity 10`, `Oec.SpawnMonster Golem_N`, `Oec.GiveItem Rifle_SR`, `Oec.GotoPhase 2` 등. 테스트 시간을 크게 줄인다 |
| 🔴 | 시드 고정 랜덤 | 방 배치·질문 선택에 `FRandomStream` + 시드 사용. 버그 재현 가능 |
| 🟡 | 질문 데이터 CSV 워크플로 | `ProjectData/*.csv`를 원본으로 두고 에디터에서 재임포트. 텍스트 편집과 리뷰가 쉬워진다 |
| 🟡 | 자동 테스트 | 인벤토리, 스택, 질문 선택 로직에 Automation Test 추가 |
| 🟢 | 저장 시스템 | `USaveGame`으로 설정, 엔딩 해금 기록 |
