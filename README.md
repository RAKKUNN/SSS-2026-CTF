# SAS-2026-CTF

**소프트웨어및시스템보안 (2026학년도 2학기) 실습 CTF 배포 저장소**

- 실습담당: TA 임우진 (공학2호관 633호)

실습 문제는 **주차별로 배포**됩니다. 해당 주차의 패키지를 받아 Docker로 실행하고, **본인 학번 기반 플래그**를 채점 서버에 제출하세요.

---

## 📅 주차별 트랙

| 주차 | 주제 | 서비스 | 접속 방법 |
| :---: | :--- | :--- | :--- |
| 2주차 | Set-UID 권한 상승 | SSH | `ssh guest@localhost -p 2222` (PW: `guest`) |
| 3주차 | Buffer Overflow | TCP | `nc localhost 9001` |
| 4주차 | BOF with Shellcode | TCP | `nc localhost 9002` |
| 5주차 | Format String Bug | TCP | `nc localhost 9003` |

> 각 주차 패키지는 해당 주차가 되면 업로드됩니다.

---

## 🚀 실습 방법 (공통)

1. **사전 준비**: Docker Desktop 또는 OrbStack 설치 (1주차 환경설정 가이드 참고)

2. **본인 학번 설정** — ⚠️ 필수
   ```bash
   cp .env.example .env
   # .env 파일을 열어 STUDENT_ID=<본인 학번> 으로 수정
   ```
   > `STUDENT_ID` 를 설정하지 않으면 `default` 플래그가 생성되어 **채점 0점** 처리됩니다.

3. **컨테이너 빌드 및 구동**
   ```bash
   docker compose up -d --build
   docker compose ps        # 서비스가 Up 상태인지 확인
   ```

4. **실습 진행 & 제출**
   - 3단계 난이도(기초 → 기본 → 심화)로 취약점을 분석/공격
   - **Main / Hidden 플래그**를 채점 서버에 제출 (형식: `FLAG{...}`)
   - 제출 플래그에는 본인 학번이 포함되어 있어야 정상 인정됩니다

---

## 🧭 난이도 및 채점

- **Level 1 (기초) / Level 2 (기본)**: 로컬 연습 (참여 점수)
- **Level 3 (심화)**: **Main Flag + Hidden Flag** — 채점 서버 제출 대상 (학번별 고유)

---

## 🔒 안내

- 플래그는 컨테이너 기동 시 `학번 + 비밀 Salt` 기반으로 **학생마다 다르게** 생성됩니다.
- 타 학생의 플래그를 제출하면 **부정행위로 자동 탐지**됩니다.
- 문의는 학과 공지 / LMS 를 참고하세요.
