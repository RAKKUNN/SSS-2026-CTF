# Track 02: Buffer Overflow (`vault_guard`)

## 1. 문제 정보 (Challenge Info)
- **분야**: 스택 버퍼 오버플로우 (Stack Buffer Overflow & ROP)
- **난이도**: `pwnable.kr` bof / passcode 급 (보통/상)
- **접속 정보**: `nc <HOST> 9001`
- **보호 기법**:
  - Canary: **Disabled** (`-fno-stack-protector`)
  - PIE: **Disabled** (`-no-pie`)
  - NX: **Enabled** (`-z noexecstack`)
- **제공 파일**: `vault_guard.c` (소스코드) 및 바이너리

---

## 2. 시나리오 및 목표 (Scenario & Objectives)
금고 보안 장비 `vault_guard`는 2가지 인증 모드를 제공합니다. 스택 프레임 메모리 레이아웃을 정밀하게 분석하여 2개의 플래그를 획득하세요.

1. **Main Flag (Stack Variable Overwrite)**:
   - Mode 1에서 버퍼 오버플로우를 발생시켜 스택 인접 변수인 `auth_token`(0xdeadbeef)과 `target_role`(0x1337c0de)을 덮어써서 메인 플래그를 획득합니다.
2. **Hidden Flag (x86_64 ROP Chain & Calling Convention)**:
   - Mode 2에서 리턴 어드레스(RET)를 변조하여 `secret_vault(key1, key2)` 함수를 호출합니다.
   - x86_64 리눅스 호출 규약(Calling Convention)에 따라 첫 번째 인자(`rdi = 0xdeadbeefcafebabe`)와 두 번째 인자(`rsi = 0x1337c0d31337c0d3`)를 가젯(`pop rdi; pop rsi; ret`)으로 세팅하는 ROP 체인을 구성하여 히든 플래그를 획득합니다.

---

## 3. 핵심 취약점 및 메모리 레이아웃 (Memory Layout)

### Mode 1 스택 구조
```
[ user_buffer (64 bytes) ] [ auth_token (4 bytes) ] [ target_role (4 bytes) ]
▲                           ▲                        ▲
0x00                        0x40 (64)                0x44 (68)
```
- Payload: `'A'*64 + p32(0xdeadbeef) + p32(0x1337c0de)`

### Mode 2 스택 구조
`diag_buf`는 `-O0` 컴파일 시 `rbp-0x50`(80B)에 배치됩니다 (gdb `disas challenge_2_rop_diagnostic`의 `lea -0x50(%rbp),%rax` 로 확인).
```
[ diag_buf (rbp-0x50 = 80 bytes) ] [ Saved RBP (8 bytes) ] [ Return Address (8 bytes) ] ...
▲                                   ▲                       ▲
0x00                                0x50 (80)               0x58 (88)  ★RET
```
- 가젯: 서버가 출력하는 `gadget_hub`(함수 시작) 기준 — `pop rdi; pop rsi; ret` = `gadget_hub+8`, 단독 `ret` = `gadget_hub+10`
- Payload: `'A'*88 + p64(gadget_hub+8) + p64(key1) + p64(key2) + p64(gadget_hub+10) + p64(secret_vault)`
- ⚠️ `secret_vault` 내부 `fopen`의 `movaps` 때문에 호출 직전 스택 16B 정렬 필요 → 단독 `ret`(`gadget_hub+10`) 1개를 끼워 정렬을 맞춤

---

## 4. LLM 활용 프롬프트 팁 (LLM Prompting Guide)

```markdown
[프롬프트 예시 1 - Main Flag]
"C 언어로 작성된 구조체에 char user_buffer[64], uint32_t auth_token, uint32_t target_role이 순서대로 선언되어 있습니다.
read(0, user_buffer, 128) 취약점을 이용해 auth_token에 0xdeadbeef, target_role에 0x1337c0de를 쓰는 파이썬 pwntools 페이로드를 작성해줘."

[프롬프트 예시 2 - Hidden Flag]
"x86_64 환경에서 diag_buf가 rbp-0x50(80B)에 있어 오프셋 88바이트로 RET를 덮어 secret_vault(0xdeadbeefcafebabe, 0x1337c0d31337c0d3)를 호출해야 합니다.
pop rdi; pop rsi; ret 가젯으로 rdi/rsi에 인자를 전달하되, secret_vault 내부 fopen의 movaps 스택 16B 정렬을 위해 단독 ret 가젯을 끼우는 64비트 ROP 체인 스크립트를 작성해줘."
```
