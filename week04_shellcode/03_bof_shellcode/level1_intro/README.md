# [Shellcode Track] Level 1: 기초 (Intro) — `shellcode_intro` (로컬 연습)

## 1. 문제 개요
쉘코드(Shellcode)가 메모리에 적재되어 CPU가 그대로 실행하는 기본 메커니즘을 익히는 입문 문제입니다. (참여 점수 · 채점 서버 제출 대상 아님)

## 2. 핵심 원리
- 버퍼에 기계어 바이트를 입력받은 뒤 함수 포인터 `((void(*)())shellcode)();` 로 CPU가 그 기계어를 직접 실행합니다.
- 64비트 리눅스 `execve("/bin/sh")` 시스템 콜 어셈블리(24B)를 직접 작성해 실행해 봅니다.

## 3. 실행 (amd64 환경에서)
> ⚠️ 맥 네이티브(`gcc`)는 arm64 Mach-O가 만들어져 x86_64 쉘코드가 동작하지 않습니다. 반드시 SEED VM / WSL2 / `--platform linux/amd64` 컨테이너에서 빌드하세요 ([`LAB_GUIDE.md`](../../LAB_GUIDE.md) 0장).
```bash
make            # shellcode_intro 빌드 + flag.txt 생성
./shellcode_intro
```
- 24바이트 execve 쉘코드를 표준입력으로 보내 셸을 얻고, 셸에서 `cat flag.txt` 로 참여 플래그를 확인하세요.

## 4. 플래그
`FLAG{shellcode_intro_practice}`  (성공 시 `flag.txt` 에서 확인)
