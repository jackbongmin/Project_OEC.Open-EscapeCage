# Git 컨벤션

## 1. 기여자 표기 규칙 (최우선)
- 커밋 메시지에 `Co-Authored-By: Claude ...` 같은 줄을 **절대 넣지 않는다.**
- PR 본문에 "🤖 Generated with Claude Code" 같은 표기를 **넣지 않는다.**
- GitHub Contributors 목록에 Claude가 올라가지 않도록, 커밋 author/committer는 항상 사용자 본인 git 설정을 사용한다.
- 적용 범위: 이 프로젝트를 포함해 Claude가 작업하는 **모든 프로젝트**.
  - 전역 설정: `~/.claude/settings.json` (`attribution` 비움, `includeCoAuthoredBy: false`)
  - 전역 지침: `~/.claude/CLAUDE.md`
  - 프로젝트 지침: 루트 `CLAUDE.md`

## 2. 브랜치
| 브랜치 | 용도 |
|---|---|
| `main` | 배포/안정 버전 |
| `develop` | 기본 작업 브랜치 |

## 3. 커밋 메시지 태그
| 태그 | 용도 |
|---|---|
| `<feat>` | 기능 구현 |
| `<add>` | 에셋 추가 |
| `<design>` | 맵·레벨 디자인 |
| `<chore>` | 정리, 설정, 무시 파일 등 |
| `<refactor>` | 동작은 그대로 두고 구조만 개선 |
| `<fix>` | 버그 수정 |
| `<docs>` | 문서 작성·수정 |

형식: `<태그> 한국어 설명`
