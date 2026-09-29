# week05_fsb 실습 실행 안내

> 📘 **단계별 실습 방법·환경 설정·GDB·접근법은 [`LAB_GUIDE.md`](LAB_GUIDE.md) 를 먼저 읽으세요.**

1. `.env.example` 를 `.env` 로 복사 후 본인 학번으로 수정
   ```
   cp .env.example .env
   # .env 파일에서 STUDENT_ID=<본인학번> 으로 수정
   ```
2. 컨테이너 빌드 및 구동
   ```
   docker compose up -d --build
   ```
3. 접속
   ```
   nc localhost 9003
   ```
4. Main/Hidden 플래그를 채점 서버에 제출 (본인 학번 플래그 확인 필수)

> Level 1(`fsb_intro`) / Level 2(`fsb_basic`) 는 로컬 연습용입니다.
> `04_fsb/level1_intro`, `level2_basic` 에서 **amd64 환경**으로 `make` 후 실행하세요(LAB_GUIDE 0장).
> Level 3(`fmt_bank`) Main/Hidden 이 채점 서버 제출 대상입니다.

---

## ⚠️ 채점 무결성 규칙 (반드시 읽을 것)

1. **플래그 문자열만 제출하면 0점.** Level 3는 **작성한 익스플로잇 코드(solve_main / solve_hidden) + 취약점 분석 보고서**로 채점합니다. 플래그는 본인 확인용 토큰일 뿐입니다.
2. **본인 학번 플래그만 유효.** 플래그는 학번(STUDENT_ID)별로 다르게 생성됩니다. `.env` 의 STUDENT_ID가 본인 학번인지 확인하세요. 타인 플래그·`docker exec`(root)·오프라인 계산 값 제출은 부정행위 로그에 기록되어 0점 및 학사 조치 대상입니다.
3. **Level 1/2 는 로컬 연습(참여 점수).** 성공 시 고정 연습 플래그(`FLAG{fsb_intro_practice}` / `FLAG{fsb_basic_practice}`)가 화면에 출력되며, 채점 서버 제출 대상이 아닙니다.
4. **보고서 필수 항목**: `%p` 탐지로 도출한 버퍼 슬롯 오프셋 근거, `%s` 임의 읽기의 주소 배치 논리, `%hn` 분할 쓰기의 조각별 출력 글자 수 계산 과정(수식).
