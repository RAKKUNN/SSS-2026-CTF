# week04_shellcode 실습 실행 안내

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
   nc localhost 9002
   ```
4. Main/Hidden 플래그를 채점 서버에 제출 (본인 학번 플래그 확인 필수)

> Level 1(`shellcode_intro`) / Level 2(`shellcode_basic`) 는 로컬 연습용입니다.
> `03_bof_shellcode/level1_intro`, `level2_basic` 에서 **amd64 환경**으로 `make` 후 실행하세요(LAB_GUIDE 0장).
> Level 3(`echo_sandbox`) Main/Hidden 이 채점 서버 제출 대상입니다.

---

## ⚠️ 채점 무결성 규칙 (반드시 읽을 것)

1. **플래그 문자열만 제출하면 0점.** Level 3는 **작성한 익스플로잇 코드(solve_main / solve_hidden) + 취약점 분석 보고서**로 채점합니다. 플래그는 본인 확인용 토큰일 뿐입니다.
2. **본인 학번 플래그만 유효.** 플래그는 학번(STUDENT_ID)별로 다르게 생성됩니다. `.env` 의 STUDENT_ID가 본인 학번인지 확인하세요. 타인 플래그·`docker exec`(root)·오프라인 계산 값 제출은 부정행위 로그에 기록되어 0점 및 학사 조치 대상입니다.
3. **Level 1/2 는 로컬 연습(참여 점수).** 성공 시 고정 연습 플래그(`FLAG{shellcode_intro_practice}` / `FLAG{shellcode_basic_practice}`)가 화면에 출력되며, 채점 서버 제출 대상이 아닙니다.
4. **보고서 필수 항목**: 24B 쉘코드의 어셈블리↔기계어 매핑, GDB로 도출한 스택 오프셋 계산 과정, Hidden의 필터 우회(인코딩+자체복호화) 원리.
