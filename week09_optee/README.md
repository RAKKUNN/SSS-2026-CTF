# 9주차 — 실습 환경 구성 (QEMU & OP-TEE)

> **소프트웨어및시스템보안 (2026-2)** · 후반부(9~14주차) 임베디드·커널·부트로더 디버깅 공용 환경  
> **실습담당**: TA 임우진 (공학2호관 633호 · `woojinim64@o.cnu.ac.kr`)

실물 보드(i.MX8MM) 없이 각자 개인 노트북(macOS / Linux / Windows WSL2)에서 **ARM TrustZone(Normal World · Secure World)** 환경을 QEMU로 구동하고, 첫 Trusted Application(`optee_example_hello_world`) 실행 및 **GDB 원격 커널 디버깅**을 수행합니다.

---

## 📦 1. 실습 환경 패키지 다운로드

부팅 이미지와 커널 심볼(`vmlinux`)이 포함된 전체 패키지(`optee_qemu_env.tar.gz`, 약 113 MB)는 **GitHub Releases (`week09`)**에서 내려받습니다.

- 🔗 **릴리스 페이지**: [GitHub Release `week09` 바로가기](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week09)
- ⬇️ **터미널 직접 다운로드 & 압축 해제 (권장)**:
  ```bash
  curl -LO https://github.com/RAKKUNN/SSS-2026-CTF/releases/download/week09/optee_qemu_env.tar.gz
  tar xzf optee_qemu_env.tar.gz
  cd optee_qemu_env
  ```

> **주의 (실행 위치 & OS 환경)**
> - 모든 명령은 압축을 푼 **`optee_qemu_env/` 폴더 안에서** 실행해야 합니다 (`run.sh`가 `./image/`를 상대경로로 참조).
> - **Windows 사용자**는 PowerShell/CMD가 아닌 **WSL2(Ubuntu) 터미널**에서, WSL 홈 디렉토리(`~`) 아래에 압축을 풀고 실행하세요.

---

## 📂 2. 디렉토리 및 문서 안내

| 파일명 | 설명 |
| :--- | :--- |
| [`READ_ME_FIRST.md`](./READ_ME_FIRST.md) | 9주차 실습 개요, 보고서 제출 양식 및 채점 무결성 안내 |
| [`INSTALL.md`](./INSTALL.md) | OS별 (`macOS` / `Linux` / `Windows WSL2`) QEMU 및 GDB 설치 명령어 |
| [`LAB_GUIDE.md`](./LAB_GUIDE.md) | **[필독]** 단계별 실습 절차 (부팅 → OP-TEE 예제 → GDB 원격 디버깅 → 제출물 · 트러블슈팅) |
| [`run.sh`](./run.sh) | QEMU 실행 러너 스크립트 (기본: TrustZone 전체 부팅 / `--debug`: 커널 디버깅 모드) |

---

## 🚀 3. 핵심 실습 3단계 요약

### ① Normal · Secure World 부팅
```bash
# 터미널 1: Normal World 부팅 (로그인: root, 암호 없음)
./run.sh

# 터미널 2: Secure World (OP-TEE) 초기화 로그 확인
tail -f image/secure_world.log
```
- 종료 방법: 부팅 터미널에서 **`Ctrl-A`** 입력 후 **`X`**

### ② 첫 Trusted Application (CA ↔ TA) 실행
```bash
# Buildroot 로그인(root) 후 실행
optee_example_hello_world
# 출력 확인:
# Invoking TA to increment 42
# TA incremented value to 43
```

### ③ GDB 원격 커널 디버깅 (`:1234`)
```bash
# 터미널 1: 커널 디버깅 모드로 실행 (:1234 포트에서 정지 대기)
./run.sh --debug

# 터미널 2: GDB 연결 (macOS는 aarch64-elf-gdb, Linux/WSL2는 gdb-multiarch)
gdb-multiarch image/vmlinux
(gdb) target remote :1234
(gdb) break start_kernel
(gdb) continue
```

---

## 📝 4. 제출물 안내 (보고서형)

아래 **스크린샷 3장**과 각 단계에 대한 간단한 설명(1~2줄)을 하나의 PDF로 묶어 LMS에 제출합니다.
1. **부팅 확인**: `buildroot login:` 화면 + `image/secure_world.log`의 `OP-TEE version: 4.11.0` 출력 화면
2. **TEE 동작 확인**: `optee_example_hello_world` 실행 후 `TA incremented value to 43` 출력 화면
3. **GDB 원격 디버깅**: GDB가 `Breakpoint 1, start_kernel ()`에서 정지한 화면

- **제출 파일명**: `[SSS]week09_학번_이름.pdf`
- **마감 기한**: LMS 과제 공지 기준
