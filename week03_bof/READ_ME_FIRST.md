# week03_bof 실습 실행 안내

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
   nc localhost 9001
   ```
4. Main/Hidden 플래그를 채점 서버에 제출 (본인 학번 플래그 확인 필수)

> Level 1(`bof_intro`) / Level 2(`bof_ret2win`) 는 로컬 연습용입니다.
> `02_bof/level1_intro`, `02_bof/level2_basic` 에서 `make` 후 직접 실행하세요.
> Level 3(`vault_guard`) Main/Hidden 만 채점 서버 제출 대상입니다.

---

## ⚠️ 채점 무결성 규칙 (반드시 읽을 것)

1. **플래그 문자열만 제출하면 0점.** Level 3 Main/Hidden은 **작성한 익스플로잇 코드(solve_main.py / solve_hidden.py) + 취약점 분석 보고서**로 채점합니다.
   플래그는 "본인이 직접 획득했음"을 증명하는 **본인 확인용 토큰**일 뿐입니다.
2. **본인 학번 플래그만 유효.** 플래그는 학번(STUDENT_ID)별로 다르게 생성됩니다. `.env` 의 STUDENT_ID가 본인 학번인지 반드시 확인하세요.
   타인 플래그 제출·`docker exec`(root)나 오프라인 계산으로 얻은 플래그 제출은 채점 서버의 부정행위 로그에 기록되어 해당 과제 0점 및 학사 조치 대상입니다.
3. **Level 1/2 는 로컬 연습(참여 점수).** 정적 연습 플래그(`FLAG{bof_intro_practice}`, `FLAG{bof_basic_practice}`)이며 채점 서버 제출 대상이 아닙니다.
4. **보고서 필수 항목**: GDB 디스어셈블리로 도출한 스택 오프셋 계산 과정(Main: 구조체 변수 오프셋 / Hidden: `diag_buf @ rbp-0x50` → 88바이트, `gadget_hub+8` 가젯, 스택 16B 정렬용 단독 `ret`)을 반드시 수록하세요.
