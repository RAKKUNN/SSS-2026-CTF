# week07_race 실습 실행 안내

> 📘 **단계별 실습 방법·환경 설정·접근법은 [`LAB_GUIDE.md`](LAB_GUIDE.md) 를 먼저 읽으세요.**

1. `.env.example` 를 `.env` 로 복사 후 본인 학번으로 수정
   ```
   cp .env.example .env
   # STUDENT_ID=<본인학번>
   ```
2. 컨테이너 빌드 및 구동 (Apple Silicon 포함 — compose 에 `platform: linux/amd64` 설정됨)
   ```
   docker compose up -d --build
   ```
3. 접속
   ```
   ssh guest@localhost -p 2223      # PW: guest
   ```
4. 권한 상승 후 `cat /root/flag_main.txt` · `cat /root/flag_hidden.txt` 로 **본인 학번** 플래그 획득.

## 채점 무결성 규칙 (필독)
- 플래그는 **학번별로 다릅니다**. 본 실습은 각자 컨테이너를 띄우는 자가 호스팅이라,
  플래그 값 자체는 로컬에서 여러 방법으로 계산/열람될 수 있습니다.
- 따라서 채점 기준은 "플래그를 보여주는 것"이 아니라 **익스플로잇 코드 + 보고서**입니다.
- 타인 플래그 제출·공유, 플래그 역산 제출 등은 **부정행위**로 처리됩니다.
