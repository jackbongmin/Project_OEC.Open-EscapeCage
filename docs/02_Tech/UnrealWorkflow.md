# Claude와 언리얼 에디터 연동

## 1. 지금 바로 가능한 것 (추가 설정 없음)
| 대상 | 가능 여부 | 방법 |
|---|---|---|
| C++ 코드 (GA, GE 로직, MMC, 서브시스템 등) | ✅ | 소스 직접 수정 |
| 컴파일 | ✅ | `Engine/Build/BatchFiles/Build.bat OECEditor Win64 Development <uproject>` 실행 (에디터가 닫혀 있거나 Live Coding 사용 시) |
| Config `.ini` (입력, 태그, 렌더링 설정) | ✅ | 텍스트 수정 |
| 데이터 테이블 원본 CSV | ✅ | `ProjectData/*.csv` 수정 → 에디터에서 재임포트 |
| `.uasset` (블루프린트, 머티리얼, GA/GE 블루프린트) | ❌ | 바이너리라 직접 수정 불가 |

## 2. Python 에디터 스크립팅을 켜면 가능한 것
UE에 기본 포함된 **Python Editor Script Plugin**을 켜면, Claude가 작성한 Python 스크립트를 에디터 안에서 실행해 에셋을 수정할 수 있다.

| 대상 | 가능 여부 | 비고 |
|---|---|---|
| 블루프린트 기본값(Class Defaults) | ✅ | 예: `GA_RifleFire`의 쿨다운, `BP_Golem_N`의 이동 속도 |
| GE 블루프린트 수치 | ✅ | Modifier 값, 지속 시간 등 |
| 데이터 테이블 행 추가·수정 | ✅ | CSV/JSON 재임포트도 자동화 가능 |
| 머티리얼 인스턴스 파라미터 | ✅ | 색, 강도, 텍스처 교체 |
| 머티리얼 그래프 노드 | ⚠️ | `MaterialEditingLibrary`로 노드 생성·연결 가능하지만 번거로움 |
| 에셋 생성, 이름 변경, 이동, 레퍼런스 검색 | ✅ | |
| 레벨에 액터 배치, 속성 변경 | ✅ | |
| **블루프린트 그래프 노드 편집** | ❌ | 공식 Python API로는 사실상 불가 |

### 실행 방식 두 가지
1. **에디터를 닫은 상태 (헤드리스)**: `UnrealEditor-Cmd.exe OEC.uproject -run=pythonscript -script=스크립트.py` 로 실행. 설정만 켜면 바로 쓸 수 있다.
2. **에디터를 켠 상태 (원격 실행)**: 프로젝트 설정 → Python → *Enable Remote Execution* 을 켜면, 실행 중인 에디터에 스크립트를 보낼 수 있다. 결과를 에디터에서 바로 볼 수 있어 편하다.

### 설정 방법 (사용자가 한 번만)
1. 에디터 → Edit → Plugins → **Python Editor Script Plugin** 활성화 → 재시작
2. Project Settings → Plugins → Python → **Enable Remote Execution** 체크 (원격 실행을 쓸 경우)

## 3. 커뮤니티 MCP 플러그인
언리얼용 MCP 서버(오픈소스)를 설치하면 Claude가 에디터를 도구처럼 직접 조작할 수 있다. 블루프린트 노드 추가까지 지원하는 것도 있다. 다만 서드파티 코드라 설치 전에 저장소를 검토해야 하고, 버전 호환(UE 5.6)을 확인해야 한다.

## 4. 권장 작업 분담
- **로직, 수치 계산**: C++ (Claude가 가장 확실하게 수정 가능)
- **밸런스 수치**: 데이터 테이블 + CSV (Claude가 텍스트로 수정, 사용자 또는 스크립트가 재임포트)
- **연출, 이펙트, UI 레이아웃, 애니메이션 연결**: 블루프린트 (사용자가 에디터에서 작업)
- 블루프린트에 숫자를 직접 박지 말고 데이터 테이블이나 C++ `UPROPERTY` 기본값으로 빼두면, Claude가 조정할 수 있는 범위가 넓어진다.
