# week06_rop 실습 실행 안내

> 📘 **단계별 실습 방법·환경 설정·GDB·접근법은 [`LAB_GUIDE.md`](LAB_GUIDE.md) 를 먼저 읽으세요.**

1. `.env.example` 를 `.env` 로 복사 후 본인 학번으로 수정
   ```
   cp .env.example .env
   # STUDENT_ID=<본인학번>
   ```
2. 컨테이너 빌드 및 구동
   ```
   docker compose up -d --build
   ```
3. 접속
   ```
   nc localhost 9004   # Main
   nc localhost 9005   # Hidden
   ```
4. 셸 획득 후 `cat flag.txt` 로 얻은 **본인 학번** Main/Hidden 플래그를 채점 서버에 제출.

> Level 1(`rop_intro`) / Level 2(`rop_basic`) 는 로컬 연습용입니다(각 폴더 README).
> **채점 무결성**: 플래그는 학번별로 다릅니다. 타인 플래그 제출/공유는 부정행위.
