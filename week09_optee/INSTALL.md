# 설치 안내 — OP-TEE QEMU 실습 환경 (9주차)

실물 보드 없이 각자 노트북에서 ARM TrustZone(Normal/Secure World)·커널·부트로더를
디버깅합니다. 필요한 건 **QEMU 하나**뿐이고, 환경 이미지는 미리 빌드돼 있습니다.

> OP-TEE QEMU는 Secure World(EL3)까지 에뮬레이션하므로 가속(HVF/KVM) 없이
> **TCG**로 동작합니다. 그래서 mac(Apple Silicon/Intel)·Linux·Windows(WSL2)에서
> **속도·결과가 동일**합니다. 부팅은 조금 느립니다(수십 초). 정상입니다.

## 0. 배포물 풀기

GitHub Release(`week09`) 또는 LMS 공지에서 `optee_qemu_env.tar.gz`를 내려받아 압축을 풉니다.
(터미널에서 직접 내려받으면 브라우저 경로 이동 없이 바로 풀 수 있습니다.)

```bash
curl -LO https://github.com/RAKKUNN/SSS-2026-CTF/releases/download/week09/optee_qemu_env.tar.gz
tar xzf optee_qemu_env.tar.gz
cd optee_qemu_env        # image/  run.sh  INSTALL.md  LAB_GUIDE.md
```

## 실행 위치 & 환경 (OS별) — 먼저 확인

모든 명령은 **압축을 푼 `optee_qemu_env/` 폴더 안에서** 실행합니다. `run.sh` 가
`image/` 를 상대경로로 참조하므로, 다른 위치에서 실행하면 부팅이 실패합니다.

| OS | 실행 환경(어디서) | 셸 |
|---|---|---|
| macOS (Apple Silicon/Intel) | 터미널(Terminal.app / iTerm2) | zsh · bash |
| Linux (Ubuntu/Debian) | 기본 터미널 | bash |
| Windows | **WSL2(Ubuntu) 터미널** (PowerShell·명령 프롬프트 아님) | bash |

> Windows 사용자는 배포물을 **WSL 홈(`~`) 안에 풀고**, WSL(Ubuntu) 터미널에서
> `./run.sh` 를 실행하세요. Windows 네이티브 셸에서는 동작하지 않습니다.

## 1. OS별 QEMU(+GDB) 설치

### macOS (Apple Silicon / Intel)
```bash
brew install qemu
# 디버깅용 GDB (gdb-multiarch가 brew core에 없어 aarch64-elf-gdb 사용):
brew install aarch64-elf-gdb
```
- 이후 실습에서 `gdb-multiarch` 자리에 **`aarch64-elf-gdb`** 를 쓰면 됩니다.

### Linux (Ubuntu/Debian 계열)
```bash
sudo apt update
sudo apt install -y qemu-system-arm gdb-multiarch
```

### Windows → WSL2 (권장)
1. PowerShell(관리자): `wsl --install -d Ubuntu` 후 재부팅, Ubuntu 초기 설정.
2. **Ubuntu(WSL) 안에서** 위 Linux 명령과 동일하게 설치:
```bash
sudo apt update
sudo apt install -y qemu-system-arm gdb-multiarch
```
- 배포물도 WSL 홈(`~`)에 풀어 WSL 안에서 `./run.sh` 를 실행하세요(네이티브 Windows보다 안정적).

## 2. 설치 확인
```bash
qemu-system-aarch64 --version      # 8.x 이상 권장
```

## 3. 실행
```bash
./run.sh            # 일반 부팅 (Normal World = 이 터미널)
./run.sh --debug    # GDB 원격 디버깅 (:1234 에서 정지 대기)
```
종료: 콘솔에서 **Ctrl-A** 누른 뒤 **X**.

자세한 실습 절차는 `LAB_GUIDE.md` 를 참고하세요.
