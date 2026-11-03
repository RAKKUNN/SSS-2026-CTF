# 10주차 — 디버깅 기초 & 보안 기법 관찰

> **소프트웨어및시스템보안 (2026-2)** · 후반부 QEMU/OP-TEE 실습  
> **실습담당**: TA 임우진 (공학2호관 633호 · `woojinim64@o.cnu.ac.kr`)

9주차에 구성한 QEMU/OP-TEE 환경에서 **OP-TEE TA의 구조**를 확인하고, **gdb로 커널을 추적**하며, **시스템 레지스터(SCTLR_EL1)**와 **KASLR**을 직접 관찰합니다.

---

## 📦 1. 실습 환경

- **9주차 환경을 그대로 사용**합니다. 새로 내려받을 패키지는 없습니다.
- 환경이 없다면 [`week09_optee/`](../week09_optee)의 안내에 따라 먼저 구성합니다.
- 모든 명령은 압축을 푼 **`optee_qemu_env/` 폴더 안에서** 실행합니다. Windows는 **WSL2(Ubuntu) 터미널**을 사용합니다.

---

## 📂 2. 문서 안내

| 파일명 | 설명 |
| :--- | :--- |
| [`READ_ME_FIRST.md`](./READ_ME_FIRST.md) | 10주차 개요, 제출 양식 및 채점 무결성 안내 |
| [`LAB_GUIDE.md`](./LAB_GUIDE.md) | **[필독]** 단계별 실습 절차와 예상 출력 (TA 구조 → 커널 추적 → 시스템 레지스터 → KASLR → 제출 · 트러블슈팅) |

---

## 🚀 3. 실습 구성

### ① OP-TEE TA 구조 관찰 (OP-TEE 모드)
```bash
./run.sh                          # 터미널 1: 부팅 후 root 로그인
tail -f image/secure_world.log    # 터미널 2: Secure World 로그
```
- 게스트에서 `ls /lib/optee_armtz/`와 `optee_example_hello_world`를 실행하고, Secure World 로그에서 TA의 처리 과정을 확인합니다.

### ② 커널 추적 · 시스템 레지스터 (디버그 모드)
```bash
./run.sh --debug                  # 터미널 1: :1234 에서 정지 대기
gdb-multiarch image/vmlinux       # 터미널 2 (macOS: aarch64-elf-gdb)
```
- `start_kernel` → `console_init` → `rest_init` 순서로 정지시키고, 레지스터와 `SCTLR_EL1`을 확인합니다.

### ③ KASLR 관찰 (OP-TEE 모드)
- OP-TEE 모드로 두 번 부팅하여 `/proc/kallsyms`의 `_text` 주소를 비교합니다.

---

## 📝 4. 제출물 안내 (보고서형)

**스크린샷 3장**과 화면별 설명을 하나의 PDF로 묶어 LMS에 제출합니다. 자세한 설명 항목은 [`LAB_GUIDE.md`](./LAB_GUIDE.md)의 5절을 확인합니다.

1. TA 목록 + hello 예제 출력 + Secure World 로그
2. gdb에서 `start_kernel` → `console_init` → `rest_init` 순서로 정지한 화면
3. 두 번 부팅한 `_text` 값 + `SCTLR_EL1` 값

- **제출 파일명**: `[SSS]week10_학번_이름.pdf`
- **마감 기한**: LMS 과제 공지 기준
