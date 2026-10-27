# [Track 07] Race Condition (TOCTOU) — 7주차 CTF

setuid-root 프로그램이 파일 권한을 **확인(access)** 한 시점과 실제로 **사용(fopen)** 하는
시점 사이의 간격(TOCTOU)을 노린다. 그 틈에 심볼릭 링크를 바꿔치기해, 일반 사용자가 쓸 수
없어야 할 `/etc/passwd` 에 root 권한으로 UID-0 계정을 주입하고 권한을 상승시킨다.

## 접속
- `ssh guest@<HOST> -p 2223`  (PW: `guest`)

## 환경 노트
- 취약 바이너리: `/usr/local/bin/vulp` (Main), `/usr/local/bin/vulp_hidden` (Hidden) — 둘 다 setuid-root.
- 레이스 타깃 디렉토리: `/home/guest/work/` (guest 소유). `fs.protected_symlinks` 설정과 무관하게 동작한다.
- 플래그: `/root/flag_main.txt`, `/root/flag_hidden.txt` (root 전용 — 권한 상승 후 열람).

## 문제 구성
- **Level 1·2 (로컬 연습)**: `level1_intro/`, `level2_basic/` — 넓은 윈도우 관찰 → 루프 자동화. 각 폴더 README 참고. (서버 제출 대상 아님)
- **Main**: `vulp` 의 짧은 윈도우를 심볼릭 링크 레이스로 이겨 `/etc/passwd` 에 UID-0 계정 주입 → 권한 상승 → `flag_main`.
- **Hidden**: `vulp_hidden` 은 윈도우가 더 짧다. 왜 단순 루프로는 불안정한지, 어떻게 하면 신뢰성 있게 이길 수 있는지 고민하라 → `flag_hidden`.

## 실행
```bash
cp .env.example .env     # STUDENT_ID=본인학번
STUDENT_ID=<본인학번> docker compose up -d --build
```

## 채점
- 익스플로잇으로 root 획득 후 얻은 **본인 학번** 플래그(Main/Hidden)를 제출.
- 배점: **Main 60% / Hidden 40%**.
- 단계별 절차·도구·접근법은 `LAB_GUIDE.md` 를 먼저 읽을 것.

> **채점 무결성**: 플래그는 학번별로 다릅니다. 채점 기준은 플래그 열람이 아니라
> **익스플로잇 + 보고서**입니다. 타인 플래그 제출/공유는 부정행위로 처리됩니다.
