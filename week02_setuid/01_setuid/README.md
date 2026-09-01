# Track 01: Set-UID (`secure_logger`)

## 1. 문제 정보 (Challenge Info)
- **분야**: Set-UID 권한 상승 (Privilege Escalation)
- **난이도**: `pwnable.kr` Toddler's Bottle 급 (보통/상)
- **접속 정보**: `ssh guest@<HOST> -p 2222` (PW: `guest`)
- **제공 파일**: `secure_logger.c`, `hidden_validator.c` (C 소스코드)

---

## 2. 시나리오 및 목표 (Scenario & Objectives)
시스템 관리자(`root`) 전용 로그 유지보수 유틸리티인 `secure_logger`와 보안 점검 바이너리 `hidden_validator`가 Set-UID 권한(`-rwsr-xr-x root root`)으로 설치되어 있습니다.
일반 계정 `guest`로 접속하여 권한 상승 취약점을 분석하고 2개의 플래그를 획득하세요.

1. **Main Flag**: `secure_logger`의 환경변수 및 명령 실행 결함을 악용하여 root 권한으로 `/root/flag_main.txt`를 탈취합니다.
2. **Hidden Flag**: `hidden_validator`의 환경 변수 및 파일 디스크립터(FD) 무결성 조건을 충족하는 익스플로잇 런처를 작성하여 `/root/flag_hidden.txt`를 획득합니다.

---

## 3. 핵심 취약점 및 원리 (Vulnerabilities)

### (1) Main Flag: PATH Hijacking & `setresuid()`
- `secure_logger.c`에서 유지보수 루틴 실행 시 `system("clean_temp_logs /var/log/sysdiag.log")`를 호출합니다.
- 절대 경로(`/usr/local/bin/clean_temp_logs`)가 아닌 상대 명령어를 호출하므로, `PATH` 환경변수를 조작(`export PATH=/tmp:$PATH`)하여 악성 `clean_temp_logs` 실행 파일을 먼저 실행하도록 유도할 수 있습니다.
- 특히 `secure_logger`는 `setresuid(geteuid(), geteuid(), geteuid())`를 호출하여 Real UID와 Effective UID를 root(0)로 일치시켰으므로, 현대 Ubuntu의 `/bin/dash` 쉘이 Set-UID 실행 시 권한을 강제로 드롭(Privilege Drop)하는 방어 기작을 무력화시킵니다.

### (2) Hidden Flag: Environment & File Descriptor Inheritance
- `/home/guest/hidden_validator`는 다음 3가지 조건을 검사합니다:
  1. `geteuid() == 0` (Set-UID 비트 활성화)
  2. `getenv("SECURE_KEY") == "SYSLAB_SETUID_VERIFICATION_2026"`
  3. `read(3, buf, ...)` -> 파일 디스크립터 3번에 `"0xCAFE_BABE_ROOT\n"` 토큰 전달
- 유닉스 프로세스 생성 메커니즘(`pipe()`, `dup2()`, `fork()`, `execve()`)을 이해하고 자식 프로세스로 열린 파일 디스크립터와 환경변수를 온전히 전달하는 런처 프로그램을 작성해야 합니다.

---

## 4. LLM 활용 프롬프트 팁 (LLM Prompting Guide)

학생들은 LLM에 다음과 같이 질의하여 논리적 해결책을 도출할 수 있습니다:

```markdown
[프롬프트 예시 1 - Main Flag]
"리눅스 C 프로그램에서 setresuid()를 호출한 후 system("clean_temp_logs /var/log/sysdiag.log")를 실행하는 Set-UID 바이너리가 있습니다. 
일반 사용자 계정에서 PATH 환경변수를 가로채어 root 쉘을 얻거나 /root/flag_main.txt를 읽는 익스플로잇 셸 스크립트를 작성해줘."

[프롬프트 예시 2 - Hidden Flag]
"Set-UID 바이너리(hidden_validator)가 실행될 때 getenv("SECURE_KEY") 값과 파일 디스크립터 3(FD 3)번 파이프로부터 특정 토큰을 검증합니다.
C 언어로 pipe()와 dup2(), execve()를 사용하여 이 조건을 만족시키며 hidden_validator를 실행하는 익스플로잇 코드를 작성해줘."
```
