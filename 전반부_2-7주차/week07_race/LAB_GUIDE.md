# 7주차 Race Condition (TOCTOU) — 실습 안내서 (LAB GUIDE)

> 정답 페이로드/완성 익스플로잇은 담겨 있지 않습니다. 아래 **스캐폴드와 개념**을 바탕으로
> 직접 완성하세요. 막히면 혼자 끙끙대지 말고 TA에게 질문하세요.

## 0. 환경 준비 (전 OS 동일)
```bash
cp .env.example .env            # STUDENT_ID=<본인학번>
docker compose up -d --build    # compose 에 platform: linux/amd64 설정됨 (Apple Silicon OK)
ssh guest@localhost -p 2223     # PW: guest
```
컨테이너를 재기동하면 `/etc/passwd` 와 작업 디렉토리가 초기화됩니다(`docker compose up -d --force-recreate`).

## 1. 핵심 개념 — TOCTOU
취약 프로그램(`vulp`)은 다음처럼 동작합니다.
```c
if (access(fn, W_OK) == 0) {   // [CHECK] "너(guest) 이거 쓸 수 있어?"  ← real uid 기준
    /* ... 아주 짧은 틈 ... */
    fp = fopen(fn, "a+");      // [USE]   실제로 연다               ← effective uid=root 기준
}
```
- `fn` 은 `/home/guest/work/XYZ` (Main) / `/home/guest/work/ZZZ` (Hidden), **심볼릭 링크**입니다.
- `access` 는 **real uid(guest)** 로 검사하므로, 링크가 `/dev/null`(내가 쓸 수 있는 것)을 가리키면 통과합니다.
- `fopen` 은 **effective uid(root)** 로 열립니다. 이 사이에 링크를 `/etc/passwd` 로 바꾸면,
  root 권한으로 `/etc/passwd` 에 쓰게 됩니다. → 이 간격이 **Race Window**.

## 2. 무엇을 쓸 것인가 — /etc/passwd
UID 0 계정 한 줄을 추가하면 그 계정으로 로그인/`su` 시 root 가 됩니다.
```
<이름>:<암호해시>:0:0::/root:/bin/bash
```
- 7개 필드 중 3번째(UID)가 `0` 이어야 합니다.
- 암호 해시는 컨테이너에서 `openssl passwd -6 <원하는비번>` 으로 만들 수 있습니다.
- (참고) SEED 고전 실습은 비밀번호 없는 계정의 magic value `U6aMy0wojraho` 를 씁니다 — 개념만 알아두세요.

## 3. Main — 비원자 루프 레이스 (스캐폴드)
심볼릭 링크를 `/dev/null` 과 `/etc/passwd` 사이로 쉼 없이 번갈아 걸고, 그 사이 `vulp` 를 반복 실행합니다.
```c
/* attack.c — TODO: 완성하라 */
#include <unistd.h>
int main(void){
    while(1){
        unlink("/home/guest/work/XYZ");
        symlink("/dev/null", "/home/guest/work/XYZ");   // access 통과용
        usleep(/* TODO */);
        unlink("/home/guest/work/XYZ");
        symlink(/* TODO: 무엇을 가리켜야 root 로 쓰게 되나? */, "/home/guest/work/XYZ");
        usleep(/* TODO */);
    }
}
```
```bash
# 다른 한편에서 vulp 에 UID-0 줄을 반복 입력하며 /etc/passwd 변경을 감시:
while :; do echo "<여기에 passwd 줄>" | /usr/local/bin/vulp >/dev/null 2>&1;
    ls -l /etc/passwd; done     # 타임스탬프/내용이 바뀌면 승리
# 승리 후: su <추가한계정>  →  cat /root/flag_main.txt
```

## 4. Hidden — 왜 루프가 불안정한가, 그리고 원자적 교체
`vulp_hidden` 은 `access` 와 `fopen` 사이에 거의 간격이 없습니다. 비원자 `unlink`+`symlink` 는
그 사이에 링크가 잠깐 **사라지는(dangling)** 구간이 생겨 실패가 잦고 느립니다.
- 힌트: 링크가 **한 순간도 사라지지 않게** 두 링크를 바꿀 수 있는 시스템 콜이 있습니다.
  `man 2 rename` 의 `RENAME_EXCHANGE` 플래그(= `renameat2`)를 조사해 보세요.
- 두 개의 링크(예: `ZZZ`→`/dev/null`, `ABC`→`/etc/passwd`)를 미리 만들고 원자적으로 교환하면
  승리 신뢰성이 크게 올라갑니다. (완성 코드는 제공하지 않습니다 — 직접 작성)
- 승리 후: `su <계정>` → `cat /root/flag_hidden.txt`.

## 5. 관찰 도구
```bash
strace -f -e trace=access,openat,symlink,renameat2 /usr/local/bin/vulp   # 호출 순서 관찰
ls -l /home/guest/work/            # 현재 심볼릭 링크가 무엇을 가리키는지
```
> ⚠️ **strace 는 '호출 순서 관찰' 용도만**이다. 리눅스는 일반 사용자가 setuid 바이너리를
> ptrace(= strace)로 추적하면 **보안상 setuid 권한 상승을 적용하지 않는다**(실행 중 euid 가
> root 로 안 올라감). 즉 strace 로는 `access`/`open` 호출 흐름만 보고, **실제 권한 상승 공격의
> 성공 여부는 strace 없이** 돌려서 확인해야 한다.

## 6. 보고서
- 양식 자유(PDF). 과정·결과 스크린샷 **필수**, 동작 원리 설명 포함.
- **Race Condition 공격을 막는 방법 1가지 이상** 설명. 예(정확히 이해하고 쓸 것):
  - **open-then-check**: `access` 로 미리 묻지 말고 먼저 `open()` 한 뒤, **열린 그 FD** 를 `fstat` 로
    검사(소유자·종류·권한)하고 **같은 FD 로만** 작업한다 → 확인 대상과 사용 대상이 반드시 같아진다.
  - **`O_NOFOLLOW`**: open 시 **경로의 마지막 요소**가 심볼릭 링크면 실패시킨다(중간 경로 요소에는 적용 안 됨).
  - **`open(O_CREAT|O_EXCL)`**: 파일이 이미 있으면 원자적으로 실패 → 기존 파일/링크를 열지 못하게 한다.
  - **권한 조기 드롭**: 꼭 필요한 순간만 높은 권한 유지.
  - 주의: `sleep` 을 없애는 것만으로는 TOCTOU 가 해결되지 않는다. 윈도우가 짧아질 뿐,
    **확인과 사용을 하나의 원자적 동작으로 묶어야** 근본 해결이다.
- 파일명 `[SSS]week07_학번_이름.pdf`, 메일 `woojinim64@gmail.com`.

## 7. 트러블슈팅
| 증상 | 원인 / 조치 |
|---|---|
| 레이스가 안 이겨짐 | 윈도우가 짧음 — 시도 횟수를 늘리거나(Main), 원자적 교체 사용(Hidden) |
| `su` 가 비번을 안 받음 | 대화형 터미널(tty)에서 실행했는지 확인 |
| Apple Silicon 에서 느림 | 정상(amd64 에뮬레이션). 상한을 넉넉히 두고 반복 |
| 포트 2223 충돌 | 기존 컨테이너 종료: `docker compose down` 후 재기동 |
| `/etc/passwd` 가 꼬임 | `docker compose up -d --force-recreate` 로 초기화 |

## 8. 참고 링크
- `man 2 access`, `man 2 open`, `man 2 symlink`, `man 2 rename`(RENAME_EXCHANGE)
- SEED Lab: Race Condition Vulnerability
- OWASP: Time-of-check Time-of-use (TOCTOU)
