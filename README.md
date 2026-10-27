# SSS-2026-CTF

**소프트웨어및시스템보안 (2026학년도 2학기) 실습 배포 저장소**

- **담당교수**: 장진수 교수님
- **실습담당**: TA 임우진 (공학2호관 633호 · `woojinim64@o.cnu.ac.kr`)

본 저장소는 **전반부(2~7주차) 시스템 해킹 CTF 실습**과 **후반부(9~14주차) ARM TrustZone · 커널 · 부트로더 디버깅 실습** 자료를 주차별로 배포합니다.

---

## 📅 주차별 실습 트랙 바로가기

### Part 2. 후반부 — 임베디드 · 커널 · 부트로더 디버깅 (QEMU & OP-TEE)

> 후반부(9~14주차)는 **공용 QEMU(ARM64) + OP-TEE 환경**을 기반으로 진행되며, 별도 플래그 없이 **실습 과정 스크린샷 + 원리 설명 보고서(PDF)** 로 채점합니다.

| 주차 | 주제 | 실행 환경 | 가이드 폴더 | 배포 패키지 (Release) |
| :---: | :--- | :---: | :---: | :---: |
| **9주차** | **실습 환경 구성 (QEMU & OP-TEE)** | QEMU (`aarch64`) + GDB | [`week09_optee/`](./week09_optee) | [📦 `week09` 다운로드](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week09) |
| 10주차 | 디버깅 실습 (레지스터 · 브레이크포인트) | QEMU + GDB | *예정* | *9주차 공용 환경 사용* |
| 11주차 | Mitigations (Stack Canary · KASLR) | QEMU + GDB | *예정* | *9주차 공용 환경 사용* |
| 12주차 | U-Boot 부트로더 디버깅 | QEMU + GDB | *예정* | *9주차 공용 환경 사용* |
| 13주차 | Linux Kernel 디버깅 (MMU · Syscall) | QEMU + GDB | *예정* | *9주차 공용 환경 사용* |
| 14주차 | 디버깅 스크립트 작성 & TA 디버깅 | QEMU + GDB | *예정* | *9주차 공용 환경 사용* |

---

### Part 1. 전반부 — 메모리 · 시스템 취약점 공략 (Docker CTF)

| 주차 | 주제 | 서비스 | 접속 방법 | 가이드 폴더 | 배포 패키지 |
| :---: | :--- | :---: | :--- | :---: | :---: |
| **2주차** | Set-UID 권한 상승 | SSH | `ssh guest@localhost -p 2222` (PW: `guest`) | [`week02_setuid/`](./week02_setuid) | [`week02`](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week02) |
| **3주차** | Buffer Overflow (BOF) | TCP | `nc localhost 9001` | [`week03_bof/`](./week03_bof) | [`week03`](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week03) |
| **4주차** | BOF with Shellcode | TCP | `nc localhost 9002` | [`week04_shellcode/`](./week04_shellcode) | [`week04`](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week04) |
| **5주차** | Format String Bug (FSB) | TCP | `nc localhost 9003` | [`week05_fsb/`](./week05_fsb) | [`week05`](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week05) |
| **6주차** | Return-to-libc / ROP | TCP | `nc localhost 9004` (Main) / `9005` (Hidden) | [`week06_rop/`](./week06_rop) | [`week06`](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week06) |
| **7주차** | Race Condition (TOCTOU) | SSH | `ssh guest@localhost -p 2223` (PW: `guest`) | [`week07_race/`](./week07_race) | [`week07`](https://github.com/RAKKUNN/SSS-2026-CTF/releases/tag/week07) |

---

## 🛠️ 후반부 (9~14주차) 실습 방법 — QEMU & OP-TEE

1. **공용 환경 패키지 다운로드 및 압축 해제** (9주차에 1회 구성 후 14주차까지 공용 사용)
   ```bash
   curl -LO https://github.com/RAKKUNN/SSS-2026-CTF/releases/download/week09/optee_qemu_env.tar.gz
   tar xzf optee_qemu_env.tar.gz
   cd optee_qemu_env
   ```
   > **Windows 사용자 주의**: 반드시 **WSL2(Ubuntu) 터미널**에서, WSL 홈(`~`) 내부에 압축을 풀고 실행하세요(PowerShell/CMD 미지원).

2. **OS별 QEMU 및 GDB 설치 (최초 1회)** — 상세 내용은 [`week09_optee/INSTALL.md`](./week09_optee/INSTALL.md) 참고
   - **macOS**: `brew install qemu aarch64-elf-gdb`
   - **Linux / Windows(WSL2)**: `sudo apt update && sudo apt install -y qemu-system-arm gdb-multiarch`

3. **모드별 실행 (`optee_qemu_env/` 폴더 내부에서 실행)**
   ```bash
   ./run.sh            # OP-TEE (TrustZone: Normal + Secure World) 부팅 모드
   ./run.sh --debug    # 커널 원격 디버깅 모드 (:1234 포트에서 GDB 접속 대기)
   ```
   - QEMU 종료: 부팅 터미널에서 **`Ctrl-A`** 입력 후 **`X`**

4. **제출 방식 (보고서형)**
   - 각 주차 [`LAB_GUIDE.md`](./week09_optee/LAB_GUIDE.md)에 명시된 필수 검증 화면 스크린샷과 설명을 하나의 PDF(`[SSS]weekNN_학번_이름.pdf`)로 작성하여 LMS에 제출합니다.

---

## 🚀 전반부 (2~7주차) 실습 방법 — Docker CTF

1. **사전 준비**: Docker Desktop 또는 OrbStack 설치
2. **본인 학번 설정** — ⚠️ 필수
   ```bash
   cp .env.example .env
   # .env 파일을 열어 STUDENT_ID=<본인 학번> 으로 수정
   ```
   > `STUDENT_ID`를 설정하지 않으면 `default` 플래그가 생성되어 **채점 0점** 처리됩니다.
3. **컨테이너 빌드 및 구동**
   ```bash
   docker compose up -d --build
   docker compose ps
   ```
4. **채점 및 무결성 안내**
   - **Level 1 / Level 2**: 로컬 연습 (참여 점수)
   - **Level 3 (Main + Hidden Flag)**: 채점 서버 제출 대상 (`학번 + 비밀 Salt` 기반 고유 플래그) + 익스플로잇 코드·분석 보고서 제출.
   - `docker exec`(=root) 직접 진입 및 타 학생 플래그 제출은 무효(0점) 처리됩니다.
