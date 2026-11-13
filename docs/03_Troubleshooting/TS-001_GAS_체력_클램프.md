# TS-001: 최대 체력이 100보다 큰 몬스터가 첫 피격에 체력이 잘리는 문제

| 항목 | 내용 |
|---|---|
| 분류 | 버그 |
| 관련 시스템 | GAS (AttributeSet, GameplayEffect) |
| 발견일 / 해결일 | 2026-11-13 / 2026-11-14 |
| 관련 커밋 | `ac3819f` |

## 1. 문제
- 몬스터 체력은 데이터 테이블(`DT_MonsterStaticData`)의 `BaseHealth`로 초기화한다. 등급이 높은 몬스터(R, SR)는 체력이 100보다 크게 설정될 수 있다.
- 이런 몬스터는 **첫 번째 피격 순간 체력이 100으로 잘려서**, 실제로 받아야 할 피해보다 훨씬 많이 깎인다.
- 초기화할 때는 문제가 보이지 않는다. 초기화는 `SetNumericAttributeBase`라서 클램프를 거치지 않고, **데미지 GE가 실행될 때만** 드러난다.

## 2. 원인 분석
- 체력 변화가 처리되는 `PostGameplayEffectExecute`를 추적했다.
- 클램프 상한이 `MaxHealth` 어트리뷰트가 아니라 **상수 100으로 하드코딩**되어 있었다. 처음 플레이어만 있을 때(최대 체력 100) 작성한 코드가 몬스터 공용 베이스(`UOecAttributeSetBase`)에 그대로 남은 것이 원인이다.

```cpp
// OecAttributeSetBase.cpp (수정 전)
if (Data.EvaluatedData.Attribute == GetHealthAttribute())
{
	SetHealth(FMath::Clamp(GetHealth(), 0.0f, 100.0f));
}
```

## 3. 해결
- 상한을 캐릭터 자신의 `MaxHealth` 어트리뷰트로 바꿨다. 플레이어와 몬스터, 이후 추가될 최대 체력 버프·디버프(정신력 단계 효과)까지 같은 코드로 처리된다.

```cpp
// OecAttributeSetBase.cpp (수정 후)
if (Data.EvaluatedData.Attribute == GetHealthAttribute())
{
	// 최대 체력 기준으로 클램프 (몬스터마다 MaxHealth가 다름)
	SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
}
```

- 함께 정리: 플레이어 `BeginPlay`에서 회복 아이템 테스트용으로 체력을 50으로 덮어쓰던 코드를 제거했다 (기본값 100 사용).

## 4. 전후 비교
측정 조건: **예시 (코드 로직 기준 계산, 인게임 미측정)**. MaxHealth 300인 몬스터에게 피해 10을 1회 가했을 때.

| 지표 | 전 | 후 |
|---|---|---|
| 피격 후 체력 | 100 | 290 |
| 실제로 깎인 체력 | 200 (의도의 20배) | 10 |
| MaxHealth > 100인 몬스터의 실질 체력 상한 | 100 | MaxHealth |

## 5. 배운 점
- 공용 베이스 클래스에 특정 캐릭터 기준의 상수를 두지 않는다. 상한·하한은 어트리뷰트로 표현한다.
- GAS에서 `SetNumericAttributeBase`(초기화)와 GE 실행은 다른 경로를 탄다. 클램프 로직은 두 경로 모두에서 테스트해야 한다.
