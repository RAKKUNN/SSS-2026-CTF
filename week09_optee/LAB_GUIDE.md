# 9주차 실습 가이드 — OP-TEE QEMU 환경 구성 & 원격 디버깅

> 소프트웨어및시스템보안 2026-2 · 실습 TA 임우진
> 목표: 각자 노트북에서 ARM TrustZone(Normal/Secure World) 환경을 띄우고,
> OP-TEE 예제를 실행하고, GDB를 원격으로 붙여본다. **10~14주차의 공용 기반.**

> 이번 주의 목표는 후반부(9~14주차) 실습에서 공통으로 사용할 환경을 각자 구성하고,
> GDB 원격 디버깅까지 한 번 수행해 보는 것입니다. 아래 절차를 순서대로 진행하고,
> 문제가 발생하면 §트러블슈팅을 먼저 확인한 뒤 TA에게 문의하세요.

### 오늘의 흐름

```
 [0] 설치          [1] 부팅           [2] OP-TEE 확인        [3] gdb 디버깅
 qemu 한 번   →   ./run.sh      →   hello (42 → 43)   →   커널에서 정지
                                                              │
                                          [4] 스크린샷 3장 → PDF 제출 ◄┘
```

### TrustZone — 두 개의 실행 영역

```
   Normal World                      Secure World
 ┌────────────────┐   ── SMC ──►   ┌────────────────┐
 │ 일반 Linux     │                │ OP-TEE (보안OS)│
 │ 앱·커널·CA     │   ◄── 전환 ──  │ TA (민감 연산) │
 └────────────────┘                └────────────────┘
        └──────── EL3 Secure Monitor (QEMU가 여기까지 흉내) ───────┘
```
비유하면 하나의 건물 안 '일반 사무실'과 '금고실'에 해당하며, 두 영역 간 전환은 지정된 통로(SMC)로만 이루어집니다.

---

## 0. 준비 — 받기 & 설치

1) 배포물(`optee_qemu_env.tar.gz`)을 GitHub Release(`week09`) 또는 LMS 공지에서 받아 풉니다.
```bash
# 터미널(macOS/Linux 기본 터미널 또는 Windows WSL2 터미널)에서 직접 받기 권장:
curl -LO https://github.com/RAKKUNN/SSS-2026-CTF/releases/download/week09/optee_qemu_env.tar.gz
tar xzf optee_qemu_env.tar.gz
cd optee_qemu_env        # image/  run.sh  INSTALL.md  LAB_GUIDE.md
```

> **실행 위치 & 환경 (중요)**
> - 모든 명령은 압축을 푼 `optee_qemu_env/` 폴더 **안에서** 실행합니다.
>   `run.sh` 가 `image/` 를 상대경로로 참조하므로, 다른 위치에서 실행하면 부팅이 실패합니다.
> - macOS·Linux는 **기본 터미널**, Windows는 **WSL2(Ubuntu) 터미널**에서 실행합니다
>   (PowerShell·명령 프롬프트 아님). Windows는 배포물을 WSL 홈(`~`)에 풀어 두세요.

2) `qemu-system-aarch64`(+디버깅용 gdb)를 설치합니다 — **OS별 명령은 `INSTALL.md`** 참고.
   - macOS: `brew install qemu aarch64-elf-gdb`  (gdb는 `aarch64-elf-gdb` 사용)
   - Linux: `sudo apt install qemu-system-arm gdb-multiarch`
   - Windows: **WSL2(Ubuntu)** 안에서 Linux와 동일하게.

> OP-TEE QEMU는 Secure World(EL3)까지 에뮬레이션해 **TCG**로 동작합니다. 부팅이
> 수십 초 걸리는 것은 정상이며, 어느 OS에서든 속도·결과가 같습니다.

---

## 1. 부팅

```bash
./run.sh
```
부팅이 진행되며 다음 순서로 로그가 올라옵니다 — TF-A(부트펌웨어) → U-Boot → Linux.

- 중간에 `WARNING: Firmware Image Package header check failed` 가 여러 줄 보여도
  **정상**입니다. (BL1이 FIP를 먼저 찾다가 없으면 개별 이미지로 자동 전환하는 과정)
- 다 올라오면 이 터미널(Normal World)에 로그인 프롬프트가 뜹니다:
```
Welcome to Buildroot, type root or test to login
buildroot login: root        # 암호 없음
#
```
- **Secure World(OP-TEE) 로그**는 다른 터미널에서 확인합니다:
```bash
tail -f image/secure_world.log
# I/TC: OP-TEE version: 4.11.0 ...   ← 이 줄이 보이면 TrustZone 스택 정상
```
- 종료: 부팅 터미널에서 **Ctrl-A** 누른 뒤 **X**.

**[제출 ①]** 로그인 프롬프트 + `secure_world.log`의 `OP-TEE version` 줄 스크린샷.

---

## 2. OP-TEE 동작 확인 — 첫 Trusted Application

로그인(`root`) 후:
```bash
# optee_example_hello_world
Invoking TA to increment 42
TA incremented value to 43
```
```
   CA (Normal World)          TA (Secure World)
 ┌──────────────────┐  ①42→  ┌──────────────────┐
 │ hello_world (앱) │ ─────► │ 값 + 1 (격리실행) │
 │                  │ ◄───── │                  │
 └──────────────────┘  ②43←  └──────────────────┘
```
- **CA**(Client App, Normal World)가 **TA**(Trusted App, Secure World)에게 값 42를
  넘기면, TA가 **격리된 보안 영역**에서 +1 해서 43을 돌려줍니다.
- `43`이 출력되면 CA↔TA 왕복(TEE)이 정상 동작하는 것입니다. 이는 모바일 지문 인증·결제가
  사용하는 구조와 동일합니다.

**[제출 ②]** `TA incremented value to 43` 출력 스크린샷.

---

## 3. 원격 디버깅 — GDB로 커널 들여다보기

> **두 가지 실행 모드** — 같은 `image/`, 목적만 다릅니다.
> - `./run.sh` = OP-TEE(TrustZone) 모드 — 1·2장에서 쓴 전체 스택.
> - `./run.sh --debug` = **커널 디버깅 모드** — OP-TEE 없이 순수 Linux를 리셋부터
>   `-S`로 멈춰 띄웁니다. gdb 브레이크포인트가 빠르고 정확하게 걸립니다(10·13주차 기본).

```
          터미널 A                         터미널 B
 $ ./run.sh --debug              $ gdb-multiarch image/vmlinux
 ┌───────────────────┐  :1234    (gdb) target remote :1234
 │ QEMU: 커널 정지   │◄─────────  (gdb) break start_kernel
 │ (gdb 접속 대기)   │  gdbstub   (gdb) continue   → start_kernel 에서 정지
 └───────────────────┘
```
> **주의:** 반드시 `--debug` 모드로 실행해야 `break start_kernel` 이 안정적으로 동작합니다.
> (OP-TEE 모드에서는 보안 부팅으로 인해 브레이크포인트가 걸리지 않을 수 있습니다.)

새 터미널에서 디버깅 모드로 부팅:
```bash
./run.sh --debug         # QEMU가 :1234 에서 '정지' 상태로 대기
```
또 다른 터미널에서 GDB 접속 (mac은 `aarch64-elf-gdb`, Linux/WSL2는 `gdb-multiarch`):
```bash
gdb-multiarch image/vmlinux
(gdb) target remote :1234
(gdb) break start_kernel
(gdb) continue
# ... 리셋부터 추적해 start_kernel 에서 정지 (몇 초) ...
# Breakpoint 1, start_kernel () at init/main.c
(gdb) info registers        # 레지스터 확인
(gdb) backtrace             # 콜스택 (start_kernel ← __primary_switched)
(gdb) continue              # 계속 진행하면 로그인까지 부팅
```
- `start_kernel`에서 멈추면, 이 지점부터가 10주차 레지스터·브레이크포인트 실습의 출발점입니다.
- `vmlinux`는 커널 심볼 파일이라 함수명·소스 단위 디버깅이 됩니다.
- (커널 주소가 매번 같도록 이 모드는 `nokaslr`로 띄웁니다 — KASLR는 11주차에서 다룹니다.)

**[제출 ③]** GDB가 `start_kernel`에서 정지한 화면 스크린샷.

---

## 4. 제출

세 스크린샷(①부팅+OP-TEE ②hello 43 ③gdb start_kernel)을 하나의 PDF로:
- 파일명: `[SSS]week09_학번_이름.pdf`
- 제출: LMS 공지 기준 마감
- 간단히: 각 단계가 무엇을 의미하는지 1~2줄씩 설명 추가

---

## 트러블슈팅

| 증상 | 원인 / 해결 |
|---|---|
| `qemu-system-aarch64: command not found` | 설치 안 됨 → `INSTALL.md` OS별 설치 |
| 부팅이 매우 느리다 | TCG 에뮬레이션이라 정상(수십 초). 기다리세요 |
| `Firmware Image Package header check failed` 반복 | **정상 노이즈**(FIP→개별 이미지 폴백). 무시 |
| `Failed to load 'uImage'` 로 U-Boot 프롬프트(`=>`)에서 멈춤 | `image/` 안에서 실행하지 않았거나 배포물이 불완전 → 폴더 안에서 `./run.sh` 재실행, 안 되면 다시 받아 풀기 |
| 종료가 안 됨 | **Ctrl-A** 뒤 **X** |
| `:1234` 포트 충돌 | 이전 QEMU가 안 꺼짐 → 해당 프로세스 종료 후 재시도 |
| (mac) `gdb-multiarch` 없음 | `brew install aarch64-elf-gdb` 후 **`aarch64-elf-gdb`** 사용 |
| (Windows) 네이티브에서 꼬임 | **WSL2(Ubuntu)** 안에서 실행 권장 |
