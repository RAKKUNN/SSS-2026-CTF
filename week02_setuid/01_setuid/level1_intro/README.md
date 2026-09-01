# [Set-UID Track] Level 1: 기초 (Intro) - `suid_cat`

## 1. 문제 개요
Set-UID 비트의 가장 기본적인 동작 원리를 이해하는 웜업(Warmup) 문제입니다.
일반 사용자 계정은 접근할 수 없는 root 소유의 보호된 파일(`/root/flag_intro.txt`)을 Set-UID 바이너리를 통해 읽는 과정을 실습합니다.

---

## 2. 핵심 이론
- **Real UID vs Effective UID**: 일반 파일 실행 시 `EUID == RUID`이지만, 파일에 `chmod 4755` (Set-UID)가 설정되면 실행 시점의 `EUID`가 파일 소유자(root: 0)로 승격됩니다.
- C 언어의 `fopen()` 등 시스템 호출은 `EUID`의 권한을 기준으로 접근을 허용합니다.

---

## 3. 실습 방법 (★ 본인 PC = 로컬 연습, 채점 컨테이너가 아님)

> 이 레벨은 **본인 호스트(WSL2 / Linux / macOS)** 에서 진행하는 **로컬 연습**입니다.
> 채점용 CTF 컨테이너의 guest 계정은 `sudo` 권한이 없어 Set-UID 비트를 직접 설정할 수 없습니다.
> 아래처럼 **연습 전용 보호 파일**을 만들어 Set-UID 원리만 체험하세요.

```bash
# 1. 바이너리 빌드
make

# 2. 연습 전용 보호 파일 생성 (root 소유, 일반 사용자 읽기 불가)
sudo sh -c 'echo "FLAG{setuid_intro_practice}" > /opt/secret_note.txt'
sudo chmod 400 /opt/secret_note.txt          # 소유자(root)만 읽기
cat /opt/secret_note.txt                       # → Permission denied (정상)

# 3. Set-UID 비트 설정 (관리자 권한 필요)
sudo chown root:root suid_cat
sudo chmod 4755 suid_cat

# 4. 일반 사용자 계정으로 보호 파일 읽기 (EUID 승격 확인)
./suid_cat /opt/secret_note.txt
```

---

## ⚠️ 주의 (채점 무결성)
`suid_cat` 은 **임의의 파일을 root 권한으로 읽어주는** 도구입니다. 따라서 이 바이너리를
채점 컨테이너의 실점수 플래그(`/root/flag_main.txt`, `/root/flag_hidden.txt`)에
겨누는 것은 **의도된 풀이가 아니며 0점 처리**됩니다. Level 3의 Main/Hidden 플래그는
반드시 `secure_logger` PATH 하이재킹 / `hidden_validator` FD·환경변수 익스플로잇으로
직접 획득해야 유효합니다. 본 레벨은 Set-UID 개념 이해용 **참여 점수** 항목입니다.

---

## 4. 연습 플래그
`FLAG{setuid_intro_practice}`  (정적 연습용 — 서버 채점 대상 아님)
