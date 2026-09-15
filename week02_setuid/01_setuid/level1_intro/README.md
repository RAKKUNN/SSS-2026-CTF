# [Set-UID Track] Level 1: 기초 (Intro) - `suid_cat`

## 1. 문제 개요
Set-UID 비트의 가장 기본적인 동작 원리를 이해하는 웜업(Warmup) 문제입니다.
일반 사용자로는 읽을 수 없는 root 소유 보호 파일을 **직접 만들고**, Set-UID 바이너리(`suid_cat`)로 그 파일을 읽어보며 EUID 승격 원리를 체험합니다.

> 📌 **숨겨진 플래그를 찾는 문제가 아닙니다.** 로컬에는 미리 만들어진 플래그 파일이 없습니다. 아래 3절에서 **본인이 보호 파일을 생성**하고, 그 안에 넣은 연습 플래그 문자열(`FLAG{setuid_intro_practice}`)을 Set-UID로 읽어내면 완료입니다. (개념 이해용 참여 점수)

---

## 2. 핵심 이론
- **Real UID vs Effective UID**: 일반 파일 실행 시 `EUID == RUID`이지만, 파일에 `chmod 4755` (Set-UID)가 설정되면 실행 시점의 `EUID`가 파일 소유자(root: 0)로 승격됩니다.
- C 언어의 `fopen()` 등 시스템 호출은 `EUID`의 권한을 기준으로 접근을 허용합니다.

---

## 3. 실습 방법 (★ 본인 호스트 = 로컬 연습, 채점 컨테이너 아님)

> 채점 컨테이너의 guest 는 `sudo` 가 없어 Set-UID 설정을 못 합니다. 이 레벨은 **본인 PC(WSL2 / Linux / macOS)** 에서 진행하세요.

### 공통 1단계 — 빌드 + Set-UID 설정
```bash
make                                 # suid_cat 빌드
sudo chown root:root suid_cat
sudo chmod 4755 suid_cat             # 맨 앞 '4' = Set-UID 비트
ls -l suid_cat                       # -rwsr-xr-x (s 비트) 확인
```

### 방법 A — /etc/shadow 읽기  (Linux / WSL2 권장 · 준비 불필요)
```bash
cat /etc/shadow                      # → Permission denied (일반 권한 불가)
./suid_cat /etc/shadow               # → EUID=0 으로 비밀번호 해시가 읽힘!
```
`/etc/shadow`(권한 0640 root:shadow)는 실제 비밀번호 해시 저장소라 일반 사용자는 못 읽습니다. Set-UID 로 EUID 가 root 가 되어 읽히는 것을 확인하세요.

### 방법 B — 직접 보호 파일 생성  (macOS 등 /etc/shadow 없는 환경)
```bash
sudo sh -c 'echo "FLAG{setuid_intro_practice}" > /opt/secret_note.txt'
sudo chmod 400 /opt/secret_note.txt
cat /opt/secret_note.txt             # → Permission denied
./suid_cat /opt/secret_note.txt      # → EUID=0 으로 읽힘!
```
> macOS 에는 `/etc/shadow` 가 없습니다. 방법 B 를 쓰거나, WSL2/Linux/컨테이너에서 방법 A 를 진행하세요.

### 완료 기준
`cat` 은 거부되는데 `./suid_cat` 으로는 읽히면 성공입니다 — EUID 승격 원리 체험 완료.

## ⚠️ 주의 (채점 무결성)
`suid_cat` 은 **임의의 파일을 root 권한으로 읽어주는** 도구입니다. 따라서 이 바이너리를
채점 컨테이너의 실점수 플래그(`/root/flag_main.txt`, `/root/flag_hidden.txt`)에
겨누는 것은 **의도된 풀이가 아니며 0점 처리**됩니다. Level 3의 Main/Hidden 플래그는
반드시 `secure_logger` PATH 하이재킹 / `hidden_validator` FD·환경변수 익스플로잇으로
직접 획득해야 유효합니다. 본 레벨은 Set-UID 개념 이해용 **참여 점수** 항목입니다.

---

## 4. 연습 플래그
`FLAG{setuid_intro_practice}`  (정적 연습용 — 서버 채점 대상 아님)
