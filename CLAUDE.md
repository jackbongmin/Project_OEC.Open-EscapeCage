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

## 문서
- 기획·설계 문서는 모두 `docs/` 폴더에서 관리한다. 목차는 [docs/README.md](docs/README.md).

## 소통
- 사용자에게 보이는 모든 텍스트는 한국어로 쓴다.
