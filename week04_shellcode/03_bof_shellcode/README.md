# Track 03: Buffer Overflow with Shellcode (`echo_sandbox`)

## 1. 문제 정보 (Challenge Info)
- **분야**: 쉘코드 주입 및 필터 우회 (Shellcode Injection & Filter Bypass)
- **난이도**: `pwnable.kr` shellcode / Toddler's Bottle 급 (중/상)
- **접속 정보**: `nc <HOST> 9002`
- **보호 기법**:
  - Canary: **Disabled** (`-fno-stack-protector`)
  - PIE: **Disabled** (`-no-pie`)
  - NX: **Disabled — Executable Stack** (`-z execstack`)
- **제공 파일**: `src/echo_sandbox.c` (소스코드) 및 컨테이너 바이너리

> 📘 단계별 실습 방법·환경 설정·GDB·접근법은 상위 폴더의 [`LAB_GUIDE.md`](../LAB_GUIDE.md) 를 먼저 읽으세요.

---

## 2. 시나리오 및 목표 (Scenario & Objectives)
`echo_sandbox`는 스택 실행 권한(`execstack`)이 켜진 환경에서 쉘코드 실행과 필터링을 테스트하는 서비스입니다. 두 모드를 분석해 각 플래그를 획득하세요.

1. **Main Flag — Classic Stack Shellcode via BOF (Mode 1)**
   - 프로그램이 출력하는 스택 버퍼 주소를 확인하고, 버퍼에 64비트 쉘코드를 주입한 뒤 **Return Address**를 버퍼 시작 주소로 변조하여 쉘을 획득합니다. (`/home/pwn/flag_main.txt`)
2. **Hidden Flag — Strict Null-Free & Plaintext Filter Bypass (Mode 2)**
   - `\x00`(NULL byte)과 `"/bin/sh"` 평문이 포함된 페이로드는 필터에 차단됩니다.
   - 쉘코드를 인코딩하고 **실행 시 스스로 복호화**하는 널 프리 쉘코드를 설계해 필터를 우회하고 히든 플래그를 탈취합니다. (`/home/pwn/flag_hidden.txt`)

---

## 3. 메모리 구조 힌트 (소스로 확인)

### Mode 1 (Main) — RET 변조
```
낮은 주소 [ char buf[64] ] ...... [ saved RBP (8) ] [ Return Address (8) ] 높은 주소
          ^ 쉘코드가 들어갈 곳                         ^ 여기를 buf 주소로 덮으면 점프
```
- `read(0, buf, 256)` 이 버퍼 크기보다 큰 입력을 허용 → 저장된 RBP·RET를 덮을 수 있습니다.
- **정확한 오프셋은 `gdb`/`objdump`로 `buf`의 `rbp` 기준 위치를 직접 확인**해 산정하세요. (완성 페이로드·오프셋 값은 제공하지 않습니다.)
- 페이로드 구성: `[ 쉘코드 ] + [ NOP 패딩 ] + [ 유출된 buf 주소(little-endian p64) ]`.

### Mode 2 (Hidden) — 직접 실행 + 필터
```c
if (validate_strict_filter(buf, n) != 0) return;   // \x00 / "/bin/sh" 차단
((void (*)())buf)();                               // 필터 통과 시 buf를 직접 실행
```
- RET 조작이 **필요 없습니다** — 필터만 통과하면 버퍼가 곧바로 실행됩니다.
- 관건은 **널·평문 없이** 원하는 동작을 하는 쉘코드를 만드는 것(인코딩 + 자체 복호화 스텁).

---

## 4. LLM 활용 프롬프트 팁 (개념 이해용)
> AI는 **개념 이해**에만 사용하고, 완성 익스플로잇을 그대로 제출하지 마세요(부정행위). 보고서에는 본인이 분석한 오프셋 근거가 있어야 합니다.

```markdown
[개념 질문 예시 1] "x86_64 리눅스에서 스택에 쉘코드를 올리고 실행하려면 어떤 컴파일/메모리 조건(NX, execstack)이 필요한지 설명해줘."
[개념 질문 예시 2] "페이로드에 널 바이트(\x00)가 들어가면 안 되는 상황에서, 쉘코드를 인코딩한 뒤 런타임에 스스로 복호화하는 디코더 스텁의 '원리'만 설명해줘. (절대주소 임베드 방식의 약점 포함)"
```
