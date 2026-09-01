# week02_setuid 실습 실행 안내

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
   ssh guest@localhost -p 2222  (PW: guest)
   ```
4. Main/Hidden 플래그를 채점 서버에 제출 (본인 학번 플래그 확인 필수)

---

## ⚠️ 채점 무결성 규칙 (반드시 읽을 것)

1. **진입은 SSH guest 전용.** 컨테이너 접속은 `ssh guest@localhost -p 2222` (PW: guest)로만 합니다.
   `docker exec ... bash`(= root) 로 들어가면 `/root` 플래그가 그냥 읽히지만, 이는 **실습·채점 대상이 아니며 0점**입니다.
   진입 직후 `id` 로 `uid=1000(guest)` 인지 확인하세요.
2. **플래그 문자열만 제출하면 0점.** Level 3 Main/Hidden은 **작성한 익스플로잇 코드 + 취약점 분석 보고서**로 채점합니다.
   플래그는 "본인이 직접 획득했음"을 잇는 **본인 확인용 토큰**일 뿐입니다.
3. **본인 학번 플래그만 유효.** 플래그는 학번별로 다르게 생성됩니다. 타인 플래그 제출·오프라인 계산 제출은
   채점 서버의 부정행위 로그에 기록되어 해당 과제 0점 및 학사 조치 대상입니다.
4. **Level 1/2 는 로컬 연습(참여 점수)** 입니다. 특히 Level 1 `suid_cat` 은 임의 파일을 root로 읽는 도구이므로
   채점 플래그(`/root/flag_main.txt`, `/root/flag_hidden.txt`)에 사용하지 마세요 — 의도된 풀이가 아닙니다.
