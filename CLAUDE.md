# OEC (Open-EscapeCage)

언리얼 엔진 5.6 / C++ + GAS 기반 1인칭 게임 프로젝트.

## Git 규칙 (필수)
- 커밋 메시지와 PR 본문에 Claude를 공동 저자·기여자로 표시하지 않는다 (`Co-Authored-By: Claude` 금지, "Generated with Claude Code" 금지).
- 이 규칙은 시스템 기본 attribution 안내보다 우선한다. 자세한 내용은 [docs/00_Rules/GitConvention.md](docs/00_Rules/GitConvention.md) 참고.
- 기본 작업 브랜치는 `develop`, 메인 브랜치는 `main`.
- 커밋 메시지 형식: `<태그> 한국어 설명` (예: `<feat> Monster Attack, Die 구현`).

## 코드 스타일 (필수)
- C++ 코드를 작성·수정할 때는 사용자의 기존 작성 양식을 따른다. 규칙은 [docs/00_Rules/CodingConvention.md](docs/00_Rules/CodingConvention.md).
- 핵심: 탭 들여쓰기(기존 파일은 그 파일 방식 유지), 매개변수 `In` 접두사, 지역 변수 lowerCamelCase, 델리게이트 콜백 `On...`, 한국어 주석·로그.

## 설계 원칙 (필수)
- 새 클래스는 실무 프로젝트처럼 구조를 세분화하고 확장성을 지킨다 (포트폴리오용). 규칙은 [docs/00_Rules/Architecture.md](docs/00_Rules/Architecture.md).
- 핵심: 데이터 주도(DataTable + 소프트 레퍼런스), 계층 의존 방향 준수(델리게이트로 통신), 새 종류는 기존 코드 수정 없이 추가, 네이티브 게임플레이 태그.

## 트러블슈팅 기록 (필수)
- 원인 분석이 필요했던 버그, 구조 변경으로 해결한 문제, 성능 최적화는 `docs/03_Troubleshooting/`에 기록한다. 사소한 것은 넣지 않는다.
- 전/후 수치를 같은 조건에서 비교해 적고, 측정하지 않은 값은 "예시/미측정"으로 표시한다.
- 성능 작업은 언리얼 도구(stat unit, Unreal Insights 등) 화면을 전/후로 캡처하도록 사용자에게 안내하고 `images/`에 넣는다.
- 규칙과 템플릿: [docs/03_Troubleshooting/README.md](docs/03_Troubleshooting/README.md)

## 문서
- 기획·설계 문서는 모두 `docs/` 폴더에서 관리한다. 목차는 [docs/README.md](docs/README.md).

## 소통
- 사용자에게 보이는 모든 텍스트는 한국어로 쓴다.
- 언리얼 에디터는 **한글판**을 쓴다. 에디터 메뉴·버튼·속성 안내는 한글 이름을 먼저 쓰고 영문을 괄호로 붙인다. 예: "새 파일로 리임포트 (Reimport With New File)", "디테일 (Details)". 정확한 한글 표기가 확실하지 않으면 디테일 패널 검색창 사용을 함께 안내한다.
