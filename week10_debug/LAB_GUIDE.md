# 10주차 실습 가이드 — 디버깅 기초 & 보안 기법 관찰 (OP-TEE 실습 마무리)

> 소프트웨어및시스템보안 2026-2 · 실습 TA 임우진
> 9주차에 구성한 `optee_qemu_env/` 환경을 **그대로** 사용합니다. 새로 내려받을 파일은 없습니다.
> 이번 주는 10~14주차에 다루던 디버깅 내용을 **압축**한 실습입니다. 각 항목은 결과를 직접 관찰하고 확인하는 데 초점을 둡니다.

> 각 단계에 **예상 출력**을 함께 적었습니다. 막히는 부분은 [트러블슈팅](#트러블슈팅)을 먼저 확인하고, 해결되지 않으면 [문의](#문의) 방법을 따릅니다.

**목표**
1. OP-TEE TA(Trusted Application)가 식별되고 호출되는 방식을 확인합니다.
2. gdb로 커널을 함수 단위로 추적하고 레지스터 값을 확인합니다.
3. KASLR(커널 주소 무작위화)로 커널 주소가 부팅마다 달라지는 것을 확인합니다.

---

## 0. 준비

- **작업 위치**: 압축을 푼 `optee_qemu_env/` 폴더 **안**에서 실행합니다(9주차와 동일).
- **호스트 gdb**: macOS는 `aarch64-elf-gdb`, Linux/WSL2는 `gdb-multiarch`를 사용합니다(9주차에 설치 완료).
- 게스트(QEMU 안의 리눅스)에는 컴파일러와 디버거가 없습니다. 디버깅은 **호스트의 gdb**로 수행합니다.
- **프롬프트 표기**: 아래 코드 블록에서 `$`는 호스트 셸, `#`는 게스트(root) 셸, `(gdb)`는 gdb 프롬프트를 나타냅니다. **이 기호는 입력하지 않고** 뒤의 명령만 입력합니다. `#`까지 함께 입력하면 주석으로 처리되어 명령이 실행되지 않습니다.
- **터미널 구성**: 1절은 터미널 2개(게스트 콘솔, Secure World 로그), 2·3절도 2개(QEMU, gdb)를 사용합니다. 1절을 마치면 게스트 콘솔에서 **Ctrl-A → X**로 QEMU를 종료한 뒤 2절로 넘어갑니다.

---

## 1. OP-TEE TA 구조 관찰

**1-1.** OP-TEE 모드로 부팅하고 root로 로그인합니다. 부팅에는 수십 초가 걸립니다.
```bash
$ ./run.sh            # 부팅 완료 후 login: root (암호 없음)
```

**1-2.** 9주차의 hello 예제를 다시 실행합니다.
```
# optee_example_hello_world
Invoking TA to increment 42
TA incremented value to 43
```

**1-3.** 설치된 TA 파일을 확인합니다.
```
# ls /lib/optee_armtz/
```
- `.ta` 파일이 약 37개 출력됩니다. 각 파일 이름이 TA의 **UUID**입니다.
- hello 예제가 호출하는 TA는 `8aaaf200-2450-11e4-abe2-0002a5d5c51b.ta`입니다. 목록에서 찾아 확인합니다.
- CA는 TA의 UUID로 세션을 열고(`TEEC_OpenSession`), 같은 UUID를 가진 TA가 Secure World에서 실행됩니다.

**1-4.** 다른 터미널(호스트, `optee_qemu_env/` 폴더)에서 Secure World 로그를 열어 두고, 게스트에서 hello 예제를 한 번 더 실행합니다.
```bash
$ tail -f image/secure_world.log
```
hello 예제를 실행할 때마다 아래 순서로 로그가 출력됩니다.
```
D/TA:  TA_CreateEntryPoint:18 has been called
D/TA:  __GP11_TA_OpenSessionEntryPoint:47 has been called
I/TA: Hello World!
D/TA:  inc_value:78 has been called
I/TA: Got value: 42 from NW
I/TA: Increase value to: 43
I/TA: Goodbye!
D/TA:  TA_DestroyEntryPoint:29 has been called
```
- `Got value: 42 from NW` → `Increase value to: 43`은 **값을 1 증가시키는 연산이 Secure World(TA) 안에서 실행되었음**을 보여 줍니다. (`NW` = Normal World)
- CA 쪽 호출과 TA 쪽 로그는 아래와 같이 대응합니다.

| CA (Normal World) | TA 쪽 로그 (Secure World) |
|---|---|
| `TEEC_OpenSession` | `TA_CreateEntryPoint` → `TA_OpenSessionEntryPoint` → `Hello World!` |
| `TEEC_InvokeCommand` | `inc_value` → `Got value: 42` → `Increase value to: 43` |
| `TEEC_CloseSession` | `Goodbye!` → `TA_DestroyEntryPoint` |

**1-5.** (4절 준비) 같은 부팅 상태에서 커널 주소를 미리 기록해 둡니다. 4절에서 1회차 값으로 사용합니다.
```
# grep -w '_text' /proc/kallsyms
```

> 참고: 이번 주에는 TA를 **직접 빌드하지 않습니다.** 배포 환경에 TA 개발 SDK가 포함되어 있지 않기 때문입니다.
> TA 작성은 팀 프로젝트(12~14주차)에서 필요한 경우에 다룹니다.

**정리 질문**
- UUID는 왜 필요한가?
- CA와 TA는 어떤 방식으로 연결되는가?

---

## 2. 커널 레지스터 · 브레이크포인트

**2-1.** 커널 디버깅 모드로 실행합니다. QEMU가 `:1234` 포트에서 정지 상태로 gdb 접속을 기다립니다.
```bash
$ ./run.sh --debug
```

**2-2.** 다른 터미널(같은 `optee_qemu_env/` 폴더)에서 커널 심볼 파일 `vmlinux`를 지정해 gdb를 실행합니다.
```bash
$ aarch64-elf-gdb image/vmlinux     # macOS
$ gdb-multiarch image/vmlinux       # Linux / WSL2
```
gdb 프롬프트가 나타나면 아래 순서로 입력합니다.
```gdb
(gdb) target remote :1234
(gdb) break start_kernel
(gdb) continue                   # start_kernel 에서 정지
(gdb) info registers pc x0 sp x1 # 주요 레지스터 확인
(gdb) x/3i $pc                   # 현재 위치의 명령 3개
(gdb) bt 1                       # 콜스택 1단계
(gdb) break console_init
(gdb) break rest_init
(gdb) continue                   # console_init 에서 정지
(gdb) bt 1
(gdb) continue                   # rest_init 에서 정지
(gdb) bt 1
```

예상 출력(일부):
```
Breakpoint 1, start_kernel () at init/main.c:912
pc             0xffff800081f007dc  0xffff800081f007dc <start_kernel>
=> 0xffff800081f007dc <start_kernel>:   paciasp
#0  start_kernel () at init/main.c:912
Breakpoint 2, console_init () at kernel/printk/printk.c:4287
Breakpoint 3, rest_init () at init/main.c:716
```

**2-3.** 확인할 내용
- 정지 순서가 `start_kernel → console_init → rest_init`인지 확인합니다.
- 각 함수의 역할: `start_kernel`은 아키텍처와 무관한 공통 C 코드의 시작점, `console_init`은 콘솔 출력 장치 초기화, `rest_init`은 PID 1(`kernel_init`)과 `kthreadd`를 생성해 부팅을 마무리합니다.
- `x/3i $pc`의 첫 명령 `paciasp`는 ARM 포인터 인증(PAC) 명령으로, 함수의 복귀 주소에 서명해 변조를 탐지합니다.
- `No such file or directory` 경고가 표시될 수 있습니다. 커널 소스 파일이 배포본에 포함되어 있지 않아 표시되는 메시지이므로 **무시해도 됩니다.** 이번 주에는 함수 단위로만 확인합니다.

---

## 3. 시스템 레지스터 — MMU 상태 확인

2-2를 마치고 `rest_init`에서 정지한 상태에서 입력합니다. (start_kernel에서 정지했을 때 입력해도 두 값은 같습니다.)
```gdb
(gdb) info registers SCTLR_EL1 TTBR1_EL1
```

- **SCTLR_EL1**: 시스템 제어 레지스터입니다. **최하위 비트(bit 0, M 비트)가 1이면 MMU가 켜져 있습니다.** 값의 마지막 16진수 자리가 홀수이면 bit 0이 1입니다.
  - 예상 출력: `SCTLR_EL1  0x200000034f4d91d` → 마지막 자리 `d`(=13, 홀수) → **MMU 켜짐**
  - 예상 출력: `TTBR1_EL1  0x420f2000`
  - 마지막 자리 `d` = 2진수 `1101` → bit 0 M(MMU)=1, bit 1 A(정렬 검사)=0, bit 2 C(데이터 캐시)=1, bit 3 SA(스택 정렬 검사)=1
- **TTBR1_EL1**: 커널 주소(상위 주소 공간)를 변환하는 페이지 테이블의 기준 주소입니다. 사용자 영역(하위 주소)은 `TTBR0_EL1`이 담당합니다.
- 시스템 레지스터는 일반 `info registers` 출력에 표시되지 않습니다. **이름을 대문자로 직접 지정**해야 값을 확인할 수 있습니다.

**정리 질문**
- start_kernel이 실행되는 시점에 MMU가 이미 켜져 있는 이유는 무엇인가?
- `pc` 값(예: `0xffff8000...`)이 상위 주소 공간에 있는 이유는 무엇인가?

---

## 4. 보안 기법 관찰 — KASLR

**KASLR**(Kernel Address Space Layout Randomization)은 커널이 적재되는 주소를 부팅마다 무작위로 바꾸는 보호 기법입니다. 공격자가 커널 주소를 예측하기 어렵게 합니다. 6주차 ROP처럼 공격 코드는 정확한 주소가 필요하므로, KASLR은 사용자 영역 ASLR의 커널 버전에 해당합니다. 주소가 유출되면 효과가 사라지므로 일반 사용자에게는 `/proc/kallsyms`의 주소를 숨기며, 이 실습에서는 root로 확인합니다.

**4-1.** 1-5에서 기록한 값이 1회차입니다. OP-TEE 모드로 **한 번 더** 부팅하여 같은 명령을 실행합니다.
```bash
$ ./run.sh            # 부팅 → login: root
```
```
# grep -w '_text' /proc/kallsyms
ffffac9177e00000 T _text        ← 예시. 값은 부팅마다 다릅니다
```
- 1-5를 진행하지 않았다면 두 번 부팅하여 값을 각각 기록합니다(`poweroff -f`로 종료한 뒤 다시 `./run.sh`).

**4-2.** 확인할 내용
- 두 부팅의 `_text` 값이 **서로 다른지** 확인합니다. 값이 다르면 KASLR이 적용되어 있는 것입니다.

**4-3.** 비교: 커널 디버깅 모드(`./run.sh --debug`)는 커널 명령줄에 `nokaslr`를 넣어 KASLR을 끕니다. 그래서 2절에서 `break start_kernel`을 설정하면 **몇 번을 실행해도 같은 주소**(`0xffff800081f007dc`)에서 정지합니다. gdb가 심볼 주소로 브레이크포인트를 설정할 수 있도록 주소를 고정하는 설정입니다.

**4-4.** 참고: **스택 카나리(Stack Canary)** 는 함수의 스택에 값을 넣어 두고, 함수가 반환되기 전에 그 값이 바뀌었는지 검사하는 기법입니다. 게스트에 컴파일러가 없어 이번 주에는 개념만 다룹니다.

---

## 5. 제출

아래 3장을 하나의 PDF(`[SSS]week10_학번_이름.pdf`)로 묶어 LMS에 제출합니다. 마감은 LMS 공지를 확인합니다.

| 번호 | 화면 | 함께 쓸 설명 |
|---|---|---|
| ① | `ls /lib/optee_armtz/` 결과(hello TA `8aaaf200-…`) + hello의 `43` 출력 + Secure World 로그 | UUID의 역할, `42 → 43`이 Secure World에서 실행됐다는 근거 |
| ② | gdb에서 `start_kernel` → `console_init` → `rest_init` 순서로 정지한 화면 | 각 함수가 부팅에서 맡는 역할 |
| ③ | 두 번 부팅한 `_text` 값 + `SCTLR_EL1` 값 | KASLR 적용 여부, M 비트로 본 MMU 상태 |

각 장마다 오른쪽 열의 내용을 **1~2줄**로 설명합니다.

---

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `target remote :1234` 연결 실패 | `./run.sh --debug`를 먼저 실행했는지 확인합니다. 이전 QEMU가 남아 있으면 종료한 뒤 다시 실행합니다 |
| 두 부팅의 `_text` 값이 같음 | KASLR 비교는 OP-TEE 모드(`./run.sh`)에서 진행합니다. 디버그 모드는 `nokaslr`로 주소가 고정됩니다 |
| `--debug` 모드에서 게스트에 로그인할 수 없음 | 정상입니다. 디버그 모드는 gdb가 접속하기 전까지 정지(`-S`) 상태입니다. 게스트 명령은 OP-TEE 모드에서 실행합니다 |
| Secure World 로그가 비어 있음 | OP-TEE 모드(`./run.sh`)로 부팅했는지, `optee_qemu_env/` 폴더에서 `tail -f`를 실행했는지 확인합니다 |
| `No such file or directory` 경고 | 커널 소스 파일이 없어 표시되는 메시지이므로 무시합니다 |
| `Invalid register 'sctlr_el1'` | 레지스터 이름은 대소문자를 구분합니다. **대문자** `SCTLR_EL1`로 입력합니다 |
| 명령을 입력해도 실행되지 않음 | 프롬프트 기호(`#`)까지 함께 입력했는지 확인합니다. 기호를 빼고 명령만 입력합니다 |
| QEMU가 종료되지 않음 | 콘솔에서 **Ctrl-A**를 누른 뒤 **X**를 누릅니다 |

---

## 문의

트러블슈팅으로 해결되지 않으면 아래 내용을 함께 적어 메일로 문의합니다.

- 받는 사람: TA 임우진 `woojinim64@o.cnu.ac.kr` (공학2호관 633호)
- 제목: `[소프트웨어및시스템보안][10주차_학번_이름] 문의내용`
- 내용: 사용 OS(macOS / Linux / Windows WSL2), 막힌 단계 번호(예: 2-2), 오류 화면 스크린샷
