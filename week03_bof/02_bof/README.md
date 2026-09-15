# Track 02: Buffer Overflow (`vault_guard`)

## 1. 문제 정보 (Challenge Info)
- **분야**: 스택 버퍼 오버플로우 (Stack Buffer Overflow — 인접 변수 정밀 덮어쓰기)
- **난이도**: `pwnable.kr` bof 급 (보통)
- **접속 정보**: `nc <HOST> 9001`
- **보호 기법**:
  - Canary: **Disabled** (`-fno-stack-protector`)
  - PIE: **Disabled** (`-no-pie`)
  - NX: **Enabled** (`-z noexecstack`)
- **제공 파일**: `vault_guard.c` (소스코드) 및 바이너리

> ℹ️ 64비트 ROP(가젯·호출 규약)는 **6주차 ROP** 주제로 다룹니다. 3주차는 **변수 덮어쓰기(Main)** 와 **return address 덮어쓰기(Level 2 ret2win)** 에 집중합니다.

---

## 2. 시나리오 및 목표 (Scenario & Objectives)
금고 보안 장비 `vault_guard`(Mode 1)의 스택 프레임 메모리 레이아웃을 정밀하게 분석하여 **Main Flag**를 획득하세요.

- **Main Flag (Stack Variable Overwrite)**:
  - Mode 1에서 버퍼 오버플로우를 발생시켜 스택 인접 변수인 `auth_token`과 `target_role`을 **동시에** 요구 값으로 덮어써서 메인 플래그를 획득합니다.
  - 각 변수의 **요구 값(Required)** 은 접속 시 프로그램이 출력해 줍니다.

---

## 3. 핵심 취약점 및 메모리 레이아웃 (Memory Layout)

### Mode 1 스택 구조 (소스로 확인)
```
[ user_buffer (N bytes) ] [ auth_token (4 bytes) ] [ target_role (4 bytes) ]
▲                          ▲                        ▲
버퍼 시작                   버퍼 끝                   +4
```
- `read()`가 버퍼 크기보다 큰 입력을 허용 → 뒤의 두 `uint32` 변수를 덮을 수 있습니다.
- **오프셋(버퍼 크기)과 요구 값**은 소스코드와 실행 시 출력으로 **직접 확인**해 페이로드를 구성하세요. (완성 페이로드는 제공하지 않습니다 — little-endian으로 4바이트씩 패킹.)

---

## 4. LLM 활용 프롬프트 팁 (LLM Prompting Guide)

```markdown
[프롬프트 예시 - Main Flag]
"packed 구조체에 char user_buffer[N] 다음 uint32_t 변수 두 개가 순서대로 있습니다.
read()로 버퍼를 넘쳐 두 변수를 각각 특정 값으로 덮는 페이로드의 '구성 방법'을 설명해줘
(오프셋 계산 방식, little-endian p32 패킹). 실제 크기·목표 값은 내가 소스/실행 출력에서 직접 확인한다."
```
