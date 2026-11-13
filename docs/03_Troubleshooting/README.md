# 트러블슈팅 기록

포트폴리오에 쓸 수 있는 문제 해결 과정을 기록한다.

## 1. 기록 기준
**기록한다**
- 원인을 찾는 과정이 있었던 버그 (재현 → 원인 분석 → 해결)
- 구조나 설계를 바꿔서 해결한 문제
- 성능 최적화 (프레임, 메모리, 로딩, 히치)

**기록하지 않는다**
- 오타, 단순 누락, 에디터 설정 실수처럼 원인이 바로 보이는 것
- 기능 추가 자체 (그건 커밋과 [ImplementationStatus](../02_Tech/ImplementationStatus.md)에서 관리)

## 2. 작성 규칙
- 파일 이름: `TS-번호_짧은제목.md` (예: `TS-001_GAS_체력_클램프.md`). 형식은 [_Template.md](_Template.md).
- **전/후 수치를 반드시 적는다.** 같은 조건(맵, 몬스터 수, 해상도, 빌드 설정)에서 측정하고, 조건도 함께 적는다.
- 측정하지 못한 수치는 "예시"나 "미측정"이라고 분명히 표시한다. 추정치를 측정값처럼 쓰지 않는다.
- 관련 커밋 해시를 적는다.

## 3. 스크린샷
성능 관련 기록은 언리얼 자체 도구 화면을 전/후로 캡처해서 `images/` 폴더에 넣는다.
- 파일 이름: `TS-번호_before_도구.png`, `TS-번호_after_도구.png`
- 문서에서: `![전](images/TS-003_before_statunit.png)`

| 보고 싶은 것 | 도구 / 명령 | 캡처 포인트 |
|---|---|---|
| 프레임 시간 (Game / Draw / GPU) | 콘솔 `stat unit`, `stat unitgraph` | 그래프가 안정된 구간 |
| FPS | `stat fps` | |
| 게임 스레드 세부 (Tick, AI 등) | `stat game`, `stat ai` | |
| 메모리 | `stat memory`, `memreport -full` | 수치 표 |
| 레벨 스트리밍 / 히치 | `stat streaming`, `stat levels` | 방 로드 순간 |
| GPU 패스별 비용 | `ProfileGPU` (Ctrl+Shift+,) | GPU Visualizer 창 |
| 프레임 전체 타임라인 | **Unreal Insights** (`-trace=cpu,gpu,frame` 실행 또는 `Trace.Start`) | 타임라인과 Timers 표 |
| 텍스처 메모리 | `stat streaming`, `ListTextures` | |

- Claude는 언리얼 화면을 직접 캡처할 수 없다. 최적화 작업 전후에 Claude가 **측정할 화면과 명령을 안내**하면, 사용자가 캡처해서 `images/`에 넣는다. (Win + Shift + S → 저장)
- 쉬핑/개발 빌드와 에디터(PIE) 수치는 다르다. 어떤 환경에서 쟀는지 적는다.

## 4. 목록
| 번호 | 제목 | 분류 | 결과 |
|---|---|---|---|
| [TS-001](TS-001_GAS_체력_클램프.md) | 최대 체력이 100보다 큰 몬스터가 첫 피격에 체력이 잘리는 문제 | 버그 / GAS | 해결 |
| [TS-002](TS-002_페이즈_스택_연결_끊김.md) | 페이즈 1 스택이 쌓이지 않아 페이즈 2로 넘어가지 않는 문제 | 버그 / 구조 | 해결 (에디터 검증 대기) |
