# 상호작용(E키) 개편과 근접 무기

작성: 2026-11-16 · 상태: C++ 완료, 에디터 확인 대기

---

## 1. 상호작용: 범위 콜리전으로 후보, 시선으로 선택

### 1.1 왜 바꿨나
| | 예전 (매 프레임 시선 레이저) | 지금 (범위 후보 + 시선 선택) |
|---|---|---|
| 작은 아이템 | 정확히 조준해야 함 | 근처에서 대충 보면 잡힘 (`MaxInteractAngle` 35°) |
| 붙어 있는 빨강/파랑 버튼 | 보는 쪽 선택 | 보는 쪽 선택 (화면 중앙에 가까운 쪽) |
| 범위 콜리전만 쓸 때의 문제 | - | 겹친 버튼 중 엉뚱한 쪽이 눌릴 수 있음 → 시선 각도로 해결 |
| 비용 | Tick 항상 켜짐, 매 프레임 라인 트레이스 | 범위 안에 대상이 있을 때만 Tick |

### 1.2 구조
```
AOecInteractableBase (아이템, 버튼, 문의 부모)
 ├─ MeshComponent
 ├─ InteractPromptWidget   ← "E" 표시
 └─ InteractRange (UOecInteractRangeComponent, 구체. Pawn과만 Overlap)

AOecPlayerCharacter
 └─ UOecInteractionComponent
     1. 캡슐이 InteractRange와 겹치면 후보 등록 / 벗어나면 제거
     2. 후보가 있을 때만 Tick: 시선 각도 안 + 가려지지 않은 대상 중 화면 중앙에 가장 가까운 하나 선택
     3. 선택이 바뀌면 SetInteractFocused(true/false) → E 표시, OnInteractTargetChanged 발행 (UI용)
     4. E 입력 → 선택된 대상의 Interact
```
- `IOecInteractableInterface`에 `CanInteract`, `SetInteractFocused`, `GetInteractLocation` 추가.
  - 열린 문은 `CanInteract = false`라 E 대상에서 빠진다.
  - `GetInteractLocation`은 메시 중심(Bounds)이라 피벗 위치와 상관없이 시선 판정이 자연스럽다.
- `SetInteractionEnabled(false)`로 연출 중 상호작용을 잠시 막을 수 있다.

### 1.3 에디터 확인
1. 기존 블루프린트(`BP_Test_RedBtn/BlueBtn`, 문, 픽업)에 `InteractRange`가 자동으로 붙는다. 플레이 전에는 따로 할 것이 없다.
2. 범위 조절: 블루프린트나 배치한 액터에서 `InteractRange` 선택 → 디테일 (Details) 패널 **구체 반경 (Sphere Radius)**. 기본 150.
   - 버튼: 단말기 앞에 섰을 때 둘 다 들어오는 정도 (100~150)
   - 문: 문 크기에 맞게 키우고 위치를 문 중앙으로 옮긴다 (문 피벗이 모서리에 있으면 범위가 한쪽으로 쏠림)
   - 범위를 눈으로 보려면 뷰포트에서 액터를 선택하면 구체 외곽선이 보인다.
3. 선택 각도: `BP_Player` → `InteractionComponent` → `Max Interact Angle` (기본 35°). 작을수록 정확히 봐야 한다.
4. 확인할 것
   - 아이템 근처에서 대충 바라보면 E가 뜨고, 주우면 E가 사라지는지
   - 빨강/파랑 버튼 앞에서 시선을 옮기면 E가 보는 버튼으로 옮겨가는지
   - 벽 너머 아이템에 E가 뜨지 않는지
   - 문을 연 뒤 E가 사라지는지

---

## 2. 근접 무기

### 2.1 흐름
```
좌클릭 → AOecMeleeWeapon::StartAttack → Fire 입력 → UOecGA_MeleeAttack
  1. 휘두르기 소리, 공격 몽타주 재생 (데이터 테이블 FireMontage)
  2. 몽타주의 애님 노티파이 SendEvent (Event.Weapon.MeleeHit) 순간
     카메라 앞으로 구체 스윕 (AttackRange, SweepRadius) → Pawn만 → 벽 너머 제외 → 최대 MaxHitCount명
     → 피해 GE (SetByCaller Data.Damage = -Damage) + 타격 이펙트·소리·카메라 흔들림
  3. 몽타주가 끝나면 종료 (그 전엔 재발동 불가 = 공격 속도는 몽타주 길이)
  - 몽타주가 없으면 바로 판정하고 AttackIntervalWithoutMontage(0.6초) 뒤 종료 → 애니메이션 없이도 테스트 가능
```
- **무기 콜리전 박스 대신 카메라 스윕을 쓴 이유**: 1인칭은 팔 애니메이션이 카메라 기준이라 무기 메시의 실제 궤적이 화면과 어긋나기 쉽다. 플레이어는 "화면 중앙에 있는 적"을 때린다고 느끼므로 카메라 기준 판정이 체감과 맞고, 몽타주마다 콜리전 구간을 맞출 필요도 없다. 그래서 기존 `HitCollision` 박스와 `Enable/DisableHitCollision` 껍데기는 지웠다.
- 판정은 휘두르기 한 번에 한 번만 한다 (노티파이가 중복돼도 안전). 노티파이 없이 몽타주가 끝나면 경고 로그를 남긴다.

### 2.2 같이 고친 것: 무기 교체 시 어빌리티 회수
- 예전에는 무기를 바꿀 때 이전 무기의 발사·장전 어빌리티를 회수하지 않아, 같은 Fire 입력에 이전 무기 어빌리티가 계속 쌓였다 (총 → 검으로 바꾸면 좌클릭에 두 어빌리티가 반응).
- `AOecPlayerCharacter::WeaponAbilityHandles`에 부여한 핸들을 저장하고, `SetWeaponState`에서 `ClearWeaponAbilities()`로 회수한다.

### 2.3 에디터 작업
1. **블루프린트**
   - `OecMeleeWeapon`을 부모로 `BP_Weapon_Sword` 생성 → `WeaponMesh`에 검 메시 지정.
   - 디테일 패널 `OEC|Weapon|Melee`: `Sweep Radius`(35), `Max Hit Count`(3). `OEC|Weapon|Effects`: 휘두르기·타격 소리, 타격 이펙트. `OEC|Feedback`: 타격 카메라 흔들림.
   - `OecGA_MeleeAttack`을 부모로 `GA_MeleeAttack` 생성 → `Damage Effect Class`에 원거리 무기와 같은 피해 GE 지정. 범위를 보고 싶으면 `Draw Debug Sweep` 체크.
2. **아이템 데이터 테이블 (`DT_ItemStaticData`)에 검 행 추가**
   - `Item Type` = 무기, `Item Actor Class` = `BP_Weapon_Sword`
   - `Damage` (예: 25), `Attack Range` (예: 200, 0이면 블루프린트 값 사용)
   - `Weapon Anim State` = 검, `Weapon Ability Class` = `GA_MeleeAttack`, `Reload Ability Class` = 비움
   - `Fire Montage` = 공격 몽타주 (비우면 애니메이션 없이 즉시 판정)
3. **공격 몽타주**에 타격 순간 노티파이 추가: 노티파이 트랙 우클릭 → 노티파이 추가 (Add Notify) → `Oec Anim Notify Send Event` → `Event Tag` = `Event.Weapon.MeleeHit`.
4. 확인: `OecGiveItem <검 코드> 1` → 퀵슬롯 장착 → 골렘 소환(`OecSpawnMonster Golem_N 1`) → 좌클릭. 출력 로그 (Output Log)에 `[GA_MeleeAttack] 휘두르기: 1명 적중`이 찍히고 골렘 체력이 줄면 성공.

### 2.4 검을 들었을 때 포즈 (ABP_Player)
현재 `ABP_Player`는 `EOecWeaponState`(맨손/검/권총/라이플)로 포즈를 고르는 **Blend Poses by EOecWeaponState** 노드와, 총 Grip에 왼손을 붙이는 **Two Bone IK**를 쓴다. C++ 쪽은 이미 연결돼 있다.
- 데이터 테이블 `Weapon Anim State` = 검 → 장착 시 `CurrentWeaponState = Sword` → 애님 인스턴스가 매 프레임 읽음
- `LeftHandIKAlpha` (C++ 신규): 무기의 `bUseLeftHandIK`가 켜져 있으면 1, 아니면 0으로 부드럽게 전환. 근접 무기는 기본 꺼짐 (한손 검). 양손 무기면 무기 블루프린트에서 `Use Left Hand IK`를 켠다.

에디터 작업:
1. **검 대기 포즈 애니메이션 준비**: 1인칭 팔 스켈레톤용 검 Idle 애니메이션 (없으면 기존 Idle을 복제해 오른손만 검 잡는 모양으로 수정하거나 에셋을 구한다).
2. `ABP_Player` → 애님 그래프 (AnimGraph) → **Blend Poses by EOecWeaponState** 노드의 `Sword` 핀에 검 Idle(또는 검 이동 블렌드 스페이스)을 연결. 핀이 없으면 노드 우클릭 → `Sword`용 핀 추가 (Add pin).
3. 왼손 **Two Bone IK** 노드의 `Alpha` 핀에 변수 `Left Hand IK Alpha`를 연결 (지금은 상수 1이거나 무기 상태로 분기돼 있을 것). 이렇게 하면 검을 들 때 왼손이 허공의 Grip 위치로 끌려가지 않는다.
4. 조준 오프셋 (Aim Offset)이 총 전용이면 검 상태에서는 건너뛰거나 검용 에임 오프셋을 따로 연결한다.
5. 공격 몽타주의 슬롯이 애님 그래프의 **Slot** 노드 이름(예: `DefaultSlot`)과 같은지 확인. 다르면 몽타주가 재생돼도 화면에 안 나온다.
6. 검 소켓: 무기는 `HandGrip_R` 소켓에 붙는다. 검이 손에서 어긋나면 스켈레톤의 `HandGrip_R` 소켓 위치·회전을 조정하거나 검 메시 피벗을 맞춘다.

> 확장 메모: 무기 종류가 계속 늘면 enum 분기 대신 **링크된 애님 레이어 (Linked Anim Layers)**로 바꾸는 것이 실무 방식이다 (무기 데이터에 애님 레이어 클래스를 넣고 장착 시 `LinkAnimClassLayers`). 지금은 4종이라 enum으로 충분하고, 샷건·양손 무기가 추가될 때 전환을 검토한다.

### 2.5 도끼(스태틱 메시) 무기 만들기와 그립 위치 잡기
`AOecWeaponBase`의 루트는 스켈레탈 메시(`WeaponMesh`)라서 스태틱 메시를 바로 넣을 수 없다. 근접 무기에는 스태틱 메시 칸 `StaticWeaponMesh`(콜리전 꺼짐)를 C++로 추가했다. 에셋: `Fab/Fire_axe/fire_axe/StaticMeshes/fire_axe`, 아이콘: `01_OEC/99_etc/04_Image/axe`.

**블루프린트**
1. 콘텐츠 브라우저에서 우클릭 → 블루프린트 클래스 (Blueprint Class) → 모든 클래스 (All Classes)에서 `OecMeleeWeapon` 선택 → `BP_Weapon_FireAxe`.
2. 컴포넌트 (Components) 패널에서 `StaticWeaponMesh` 선택 → 디테일의 스태틱 메시 (Static Mesh)에 `fire_axe`. `WeaponMesh`(스켈레탈)는 비워 둔다.
3. 클래스 디폴트 (Class Defaults): `Sweep Radius`, `Max Hit Count`, 소리·이펙트. `Use Left Hand IK`는 **꺼 둔다**. 왼손 IK는 스켈레탈 메시의 `Grip` 소켓을 찾는데, 스태틱 메시 무기는 그 소켓이 없다.

**그립 위치 원리**
- 장착 시 무기 액터의 루트가 플레이어 메시의 `HandGrip_R` 소켓에 그대로 붙는다 (`SnapToTargetNotIncludingScale`).
- 그래서 **`StaticWeaponMesh`의 상대 위치·회전 = 손에서 도끼가 어떻게 잡히는지**다. 무기마다 이 값만 바꾸면 되고, 소켓(`HandGrip_R`)은 총과 같이 쓰므로 건드리지 않는다.
- 목표: 오른손 손바닥 안에 도끼 자루가 오고, 날이 앞·위를 향하게. 도끼 메시 피벗이 자루 끝이 아니면 위치로 보정한다.

**값 찾는 법 A: 플레이 중 조정 후 복사 (추천, 실제 1인칭 화면 기준)**
1. `OecGiveItem <도끼 코드> 1`로 받아서 장착.
2. **F8**(빙의 해제, Eject) → 아웃라이너 (Outliner)에서 `BP_Weapon_FireAxe_C_0` → `StaticWeaponMesh` 선택.
3. 뷰포트 기즈모(W 이동 / E 회전)로 손에 맞게 조정. 1인칭 카메라 시점 확인은 다시 F8로 빙의.
4. 디테일의 **트랜스폼 (Transform)** 제목 우클릭 → **복사 (Copy)**.
5. 플레이 종료 → `BP_Weapon_FireAxe` → `StaticWeaponMesh` → 트랜스폼 우클릭 → **붙여넣기 (Paste)** → 컴파일·저장.

**값 찾는 법 B: 스켈레톤 미리보기 (손 모양만 대략 확인)**
- 플레이어 팔 스켈레톤 에셋 → 스켈레톤 트리 (Skeleton Tree)에서 `HandGrip_R` 우클릭 → **프리뷰 애셋 추가 (Add Preview Asset)** → `fire_axe`. 손에 어떻게 붙는지 볼 수 있지만, 여기서 바꾸는 값은 미리보기 전용이라 블루프린트에 다시 옮겨 적어야 한다.

**아이템 데이터 (`DT_ItemStaticData`) 행 추가** (예: 행 이름·`Item Code` = `Weapon_FireAxe`)
| 항목 | 값 |
|---|---|
| Item Name | 소방 도끼 |
| Item Icon | `axe` |
| Item Type | 무기 |
| Item Actor Class | `BP_Weapon_FireAxe` |
| Damage | 30 (예시) |
| Attack Range | 180 (예시, 0이면 블루프린트 값) |
| Weapon Anim State | 검 (근접 포즈 공유) |
| Weapon Ability Class | `GA_MeleeAttack` |
| Reload Ability Class | 비움 |
| Fire Montage | 공격 몽타주 (없으면 비움 → 즉시 판정) |

확인: `OecGiveItem Weapon_FireAxe 1` → 퀵슬롯으로 장착 → 골렘 소환 → 좌클릭 → 출력 로그 `[GA_MeleeAttack] 휘두르기: 1명 적중`.

### 2.6 남은 것
- 근접 무기를 들었을 때 무기 패널 탄약 표시 숨기기 (지금은 이전 총의 탄약 숫자가 남을 수 있음)
- 연속 공격(콤보), 막기는 M4 이후
