# [Track 05] Return-to-libc / ROP — 6주차 CTF

NX(실행 불가 스택)가 켜진 64비트 바이너리에서, 스택에 쉘코드를 올리는 대신
**이미 메모리에 적재된 libc 함수를 재사용(Return-to-libc)** 하여 `system("/bin/sh")` 를 실행합니다.

## 접속
- **Main**  : `nc <HOST> 9004`
- **Hidden**: `nc <HOST> 9005`

## 보호 기법
| Canary | PIE | NX | ASLR |
|---|---|---|---|
| 없음 | 없음(no-PIE) | **있음** | 커널 기본(on) |

## 문제 구성
- **Level 1·2 (로컬 연습)**: `level1_intro/`, `level2_basic/` — 가젯/체이닝 기초. 각 폴더 README 참고. (서버 제출 대상 아님)
- **Main (9004)**: 배너가 **libc 앵커 주소(stdout)** 를 1개 알려줍니다. 이를 기준으로 libc 베이스를 복원해 `system("/bin/sh")` 체인을 구성하세요.
- **Hidden (9005)**: 힌트가 없습니다. **메모리 릭(puts)** 으로 libc 주소를 직접 유출한 뒤, 2단계로 `system("/bin/sh")` 를 호출하세요(ASLR 우회).

## 실행
```bash
cp .env.example .env     # STUDENT_ID=본인학번
STUDENT_ID=<본인학번> docker compose up -d --build
```

## 제출
- 셸 획득 후 `cat flag.txt` 로 얻은 **본인 학번** 플래그(Main/Hidden)를 채점 서버에 제출.
- 상세 절차·도구·트러블슈팅은 `LAB_GUIDE.md` 참고.

> **채점 무결성**: 플래그는 학번별로 다릅니다. 타인 플래그 제출/공유는 부정행위로 처리됩니다.
