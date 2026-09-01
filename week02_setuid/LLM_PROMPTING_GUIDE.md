# 소프트웨어및시스템보안 CTF: LLM 협업 문제 해결 가이드 (Prompting & Learning Guide)

본 문서는 **소프트웨어및시스템보안** 교과목의 시스템 해킹(Pwnable) 실습 과제를 수행할 때, **LLM(대형 언어 모델)을 인공지능 튜터 및 페어 프로그래머로 활용하여 깊이 있는 원리를 학습하고 문제를 주도적으로 해결하는 방법**을 안내합니다.

---

## 1. LLM 협업 문제 해결의 기본 철학

단순히 "이 문제 풀어줘"라고 질문하면 LLM이 부정확한 가정을 하거나 잘못된 오프셋을 생성할 수 있습니다. 
시스템 해킹은 **바이트 단위의 정확성(Precision)**과 **아키텍처/컴파일러 세부 사항(ABI, Calling Convention, Stack Alignment)**이 일치해야 성공합니다.

따라서 학생은 다음과 같은 **3단계 구조화된 프롬프트 워크플로우**를 사용해야 합니다:
1. **취약점 식별 단계 (Vulnerability Identification)**: C 소스코드와 메모리 보호기법(`checksec`)을 제공하여 취약점 지점 규명.
2. **수학적 오프셋 및 제약 분석 단계 (Offset & Constraints Calculation)**: 스택 프레임, 구조체 패딩, 레지스터 호출 규약을 수학적으로 계산.
3. **익스플로잇 스크립트 작성 및 디버깅 단계 (Exploit Synthesis & pwntools)**: 단계별 페이로드를 생성하고 gdb 디버깅 출력값을 피드백하여 미세 조정.

---

## 2. 트랙별 추천 프롬프트 템플릿

### Track 01: Set-UID (`secure_logger`)

#### [Main Flag: PATH Hijacking & Privilege Drop Bypass]
```markdown
# 역할: 시스템 보안 전문가
# 상황:
리눅스 환경에서 SUID root 권한으로 실행되는 C 프로그램(secure_logger.c)이 있습니다.
유지보수 함수에서 `setresuid(geteuid(), geteuid(), geteuid());`를 실행한 후 `system("clean_temp_logs /var/log/sysdiag.log");`를 호출합니다.

# 질문:
1. Ubuntu의 /bin/dash가 SUID 실행 시 권한을 드롭하는 원리와, 이 코드에서 `setresuid()`가 권한 드롭을 무력화하는 이유를 설명해줘.
2. 일반 사용자 계정(guest)에서 PATH 환경변수를 조작하여 /root/flag_main.txt를 탈취하는 셸 스크립트(solve_main.sh)를 단계별로 작성해줘.
```

#### [Hidden Flag: Environment & File Descriptor Inheritance]
```markdown
# 역할: 시스템 프로그래밍 전문가
# 상황:
SUID 바이너리 `hidden_validator`가 실행될 때 다음 3가지를 검사합니다:
1. `geteuid() == 0`
2. `getenv("SECURE_KEY")`가 "SYSLAB_SETUID_VERIFICATION_2026"인지 검사
3. 파일 디스크립터 3(FD 3)번 파이프로부터 "0xCAFE_BABE_ROOT\n" 토큰을 읽음

# 질문:
부모 프로세스에서 `pipe()`, `dup2(p[0], 3)`, `write()`, `execve()`를 호출하여 위 3가지 조건을 완벽히 만족하며 hidden_validator를 실행하는 C 언어 익스플로잇 런처를 작성해줘.
```

---

### Track 02: Buffer Overflow (`vault_guard`)

#### [Main Flag: Stack Variable Overwrite]
```markdown
# 역할: 바이너리 익스플로잇 전문가
# 코드 구조:
struct __attribute__((packed)) {
    char user_buffer[64];
    volatile uint32_t auth_token;
    volatile uint32_t target_role;
} session;

# 상황:
read(0, session.user_buffer, 128) 취약점이 있습니다.
auth_token을 0xdeadbeef, target_role을 0x1337c0de로 변경해야 합니다.

# 질문:
1. 구조체 내 변수들의 메모리 오프셋을 계산해줘.
2. 리틀 엔디언 바이트 패킹(struct.pack('<I', ...))을 적용하여 원격 서버(nc host 9001)에 전송할 파이썬 pwntools 스크립트를 작성해줘.
```

#### [Hidden Flag: 64-bit ROP & Calling Convention]
```markdown
# 역할: Pwnable ROP 엔지니어
# 상황:
x86_64 리눅스 바이너리에서 64바이트 버퍼 오버플로우가 발생합니다.
- Return Address 오프셋: 72바이트 (64바이트 버퍼 + 8바이트 Saved RBP)
- 호출할 목표 함수: `secret_vault(uint64_t key1, uint64_t key2)`
- 요구 인자: key1 = 0xdeadbeefcafebabe, key2 = 0x1337c0d31337c0d3
- 사용 가능한 가젯: `gadget_hub` (`pop rdi; pop rsi; ret`)

# 질문:
x86_64 Calling Convention(RDI, RSI)에 맞추어 스택에 ROP 체인을 배치하는 원리를 다이어그램으로 그리고, 파이썬 완성형 익스플로잇 코드를 작성해줘.
```

---

### Track 03: BOF with Shellcode (`echo_sandbox`)

#### [Main Flag: Classic Stack Shellcode]
```markdown
# 역할: 쉘코드 개발자
# 상황:
x86_64 Ubuntu 환경, NX Disabled (execstack), No Canary.
서버가 스택 버퍼의 시작 주소(예: 0x7fffffffdc00)를 출력해주고 64바이트 버퍼에 256바이트 입력을 받습니다.

# 질문:
1. 24바이트 길이의 64비트 execve("/bin/sh", 0, 0) 쉘코드를 어셈블리 및 바이트스트림으로 작성해줘.
2. 스택 버퍼 주소로 리턴 어드레스(오프셋 72)를 덮어써서 쉘을 획득하는 파이썬 스크립트를 작성해줘.
```

#### [Hidden Flag: Null-Free XOR Decoder Stub]
```markdown
# 역할: 고급 악성코드 분석 및 쉘코드 난독화 전문가
# 제약 조건:
서버의 입력 필터가 페이로드 전체에서 널 바이트(\x00)와 문자열 "/bin/sh"을 탐지하여 차단합니다.
실행 가능한 스택 버퍼 주소는 사전에 알고 있습니다.

# 질문:
1. 원본 쉘코드를 0x5a로 XOR 암호화했을 때 널 바이트와 금지 문자열을 완전히 회피할 수 있는 이유를 설명해줘.
2. 널 바이트가 전혀 포함되지 않는 x86_64 XOR 디코더 스텁(Decoder Stub) 어셈블리를 작성하고, 메모리 상에서 복호화 후 실행하는 전체 페이로드 빌더를 파이썬으로 작성해줘.
```

---

### Track 04: Format String Bug (`fmt_bank`)

#### [Main Flag: Arbitrary Memory Read via `%s`]
```markdown
# 역할: 포맷 스트링 취약점 분석가
# 상황:
x86_64 환경에서 `printf(memo)` 취약점이 존재합니다.
전역 변수 `secret_vault_key`의 주소가 0x4040a0으로 고정되어 있고, `memo` 버퍼는 스택 오프셋 6번에 위치합니다.

# 질문:
1. x86_64 레지스터 인자 전달(RSI, RDX, RCX, R8, R9)과 스택 오프셋의 관계를 설명해줘.
2. Direct Parameter Access(`%7$s`)와 주소 배치를 활용하여 0x4040a0에 저장된 64비트 데이터를 읽어오는 페이로드를 설계해줘.
```

#### [Hidden Flag: Arbitrary Memory Write via `%hn`]
```markdown
# 역할: 포맷 스트링 익스플로잇 엔지니어
# 상황:
전역 변수 `is_vip_manager`(주소: 0x4040b0)의 값을 32비트 값 `0x1337beef`로 변경해야 합니다.
`memo` 버퍼는 스택 오프셋 6번입니다.

# 질문:
1. `%hn`(2바이트 쓰기)을 사용하여 하위 2바이트(0xbeef = 48879)와 상위 2바이트(0x1337 = 4919)를 순서대로 쓰는 문자 수 계산식을 유도해줘.
2. 포맷 스트링 헤더 길이와 8바이트 주소 정렬을 고려하여 최종 익스플로잇 페이로드를 생성하는 파이썬 코드를 작성해줘.
```

---

## 3. Flag + Hidden 2단계 평가의 교육적 가치

| 단계 | 인증 대상 | 학생이 입증하는 역량 | LLM 활용 수준 |
| :--- | :--- | :--- | :--- |
| **Main Flag (60%)** | 기초 취약점 트리거 | - 기본 버퍼 오버플로우, SUID 경로 조작, 포맷 스트링 읽기<br>- 취약점 발견 및 표준 페이로드 작성 능력 | 개념 질의 및 기본 pwntools 템플릿 생성 |
| **Hidden Flag (40%)** | 심화 제약 조건 해결 | - 64비트 Calling Convention ROP 체이닝<br>- Null-free 자체 복호화 쉘코드 공학<br>- 멀티바이트 정밀 메모리 변조 (%hn split-write)<br>- 유닉스 프로세스 간 IPC/FD 상속 원리 이해 | 수학적 오프셋 계산 검증 및 커스텀 어셈블리 합성 |
