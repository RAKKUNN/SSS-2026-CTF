# 5주차 실습 안내서 — Format String Bug (FSB)

> 이 문서는 **어디서·어떤 명령으로 빌드·실행하는지**와 각 단계를 **어떻게 분석·접근하는지** 를 안내합니다.
> Main/Hidden의 완성된 정답 페이로드(정확한 오프셋·글자 수·주소 배치)는 담고 있지 않습니다. 값은 여러분이 직접 분석해 채워 넣으세요.

---

## 0. 실행 환경

- FSB의 오프셋(인자 슬롯)과 주소는 **CPU 아키텍처·컴파일 결과에 따라 달라집니다.** x86_64 리눅스 기준으로 실습하세요.
- ⚠️ **맥에서 그냥 `gcc`/`make` 하면 안 됩니다.** 애플 실리콘은 arm64 + Mach-O라 `-no-pie`·오프셋·주소가 교안과 전혀 맞지 않습니다.
- **채점 대상 Level 3(`fmt_bank`)** 는 compose에 `platform: linux/amd64` 가 지정돼 **어느 OS에서도 동일하게** 동작합니다(걱정 불필요).

**Level 1·2를 어느 OS에서든 동일하게 amd64로 빌드하는 방법 (아래 중 하나):**

1. **SEED 2.0 VM(Ubuntu 22.04) 또는 Windows WSL2(Ubuntu)** 안에서 그대로 `make` — `gdb` 가 이미 있어 가장 편합니다.
2. **VM이 없으면 amd64 컨테이너 한 줄** — Mac·Windows·Linux 어디서든 결과가 동일합니다:
   ```bash
   # 04_fsb 폴더에서 실행 (실습 런타임과 동일한 ubuntu:22.04)
   docker run --rm -it --platform linux/amd64 -v "$PWD":/w -w /w ubuntu:22.04 bash
   # ↓ 컨테이너 안에서 (최초 1회):
   apt update && apt install -y gcc gdb make python3 file
   cd level1_intro && make && ./fsb_intro     # Level 1
   cd ../level2_basic && make && ./fsb_basic   # Level 2
   ```
   > 매번 재설치가 싫으면 `--rm`을 빼고 `--name fsbdbg`를 붙여 실행한 뒤, 다음부터는 `docker start -ai fsbdbg`로 재접속하세요.

빌드 전 환경 확인:
```bash
uname -m                 # x86_64 여야 함 (aarch64/arm64면 위 컨테이너 사용)
file ./fsb_intro         # "ELF 64-bit ... x86-64" 여야 정상 (Mach-O면 환경 잘못됨)
```

---

## 1. 권장 학습 순서

1. **Level 1 (`fsb_intro`)** — `%p` 로 스택 값을 유출(정보 누출)하는 감 익히기
2. **Level 2 (`fsb_basic`)** — `%n` 으로 전역 변수에 원하는 값 쓰기(임의 쓰기)
3. **Level 3 Main (`fmt_bank` Mode 1→3)** — `%s` 임의 읽기로 비밀 키 유출 → 채점 대상
4. **Level 3 Hidden (`fmt_bank` Mode 1→2)** — `%hn` 분할 쓰기로 VIP 조건 만족 → 채점 대상(심화)

---

## 2. 포맷 스트링 기초 — 왜 뚫리나

```c
printf(user_input);          // 취약: 사용자 입력이 곧 포맷 문자열
printf("%s", user_input);    // 안전: 포맷 문자열 고정
```
- 사용자가 `%p %p %p ...` 를 넣으면 `printf` 는 인자가 넘어온 줄 알고 레지스터·스택을 읽어 출력합니다 → **정보 누출**.
- `%n` 계열은 "지금까지 출력한 글자 수" 를 **주소에 써넣습니다** → **임의 메모리 쓰기**.

**x86_64 인자 매핑:** 가변 인자 1~5번은 레지스터(`rsi, rdx, rcx, r8, r9`), **6번째부터 스택**. 입력 버퍼가 스택에 있으면, 버퍼 자체가 어느 인자 슬롯(`%k$`)에 대응하는지 찾는 게 출발점입니다.

**오프셋(슬롯) 탐지법:** 알아보기 쉬운 마커를 앞에 두고 `%p` 를 여러 개 보낸 뒤, 마커 값이 몇 번째 `%p` 에 나타나는지 확인합니다.
```
입력:  AAAAAAAA.%6$p.%7$p.%8$p.%9$p. ...
관찰:  0x4141414141414141 이 나타난 %k$p 의 k 가 버퍼 시작 슬롯
```

---

## 3. Level 1 — `fsb_intro` (로컬 연습)

- 목표: 스택에 있는 `secret_token`(`0x1337cafebeefdead`)을 `%p` 로 유출한 뒤, 2단계 입력에 그 값을 제출.
- 주의: `secret_token` 은 스택 **상위 슬롯**(대략 17번째 근처)에 있어, `%p` 를 몇 개만 보내면 안 보입니다. **넉넉히**(예: `%p.` × 20 이상) 보내 값을 찾으세요.
- 성공 시 `FLAG{fsb_intro_practice}` 출력.

접근 스캐폴드(직접 완성):
```python
# 1) "%p." 를 20개 이상 보낸다  (read 한도 63바이트 주의)
# 2) 출력에서 0x1337cafebeefdead 를 찾는다
# 3) 그 값을 hex(0x 없이)로 2단계 입력에 제출
```

---

## 4. Level 2 — `fsb_basic` (로컬 연습, 임의 쓰기)

- 목표: 전역 변수 `target` 을 정확히 `4` 로 만들기. 배너에 `&target` 이 출력됩니다.
- 핵심: 정확히 4글자 출력 후 `%n` 으로 `&target` 에 기록. 버퍼가 놓인 슬롯을 위 탐지법으로 찾아 `%k$n` 의 `k` 를 정하고, 주소가 그 슬롯에 오도록 앞부분 길이를 8의 배수로 정렬합니다.
- 성공 시 `FLAG{fsb_basic_practice}` 출력.

접근 스캐폴드(직접 완성):
```python
# payload = b"%4c" + b"%<k>$n" + <정렬 패딩> + p64(target_addr)
#   - k, 패딩 길이는 오프셋 탐지 결과로 산정
```

---

## 5. Level 3 Main — `fmt_bank` Mode 1 → Mode 3 (원격, 채점)

접속: `nc localhost 9003`

- 배너가 `&secret_key`(= `&secret_vault_key`) 주소를 출력합니다.
- Mode 1(피드백)에서 **그 주소를 페이로드에 8바이트로 배치**하고, 그 주소가 놓인 슬롯을 `%k$s` 로 역참조하면 저장된 키가 출력됩니다.
- ⚠️ 키는 **리틀엔디언**으로 저장돼 화면엔 바이트가 뒤집혀 보입니다(예: 논리값 "SYSLAB26"이 `62BALSYS`로 출력). 유출 8바이트를 `struct.unpack("<Q", raw)` 로 정수(`0x5359534c41423236`)로 복원해 **Mode 3(Unlock Vault)** 에 hex로 제출하면 Main Flag.
- 성공 시 `FLAG{fsb_arbitrary_memory_leak_secret_revealed_<학번>_<해시>}`.

TCP 서비스이므로 응답이 조각나 올 수 있습니다. 프롬프트 문자열이 나타날 때까지 누적 수신하는 보일러플레이트를 쓰세요:
```python
import socket, struct, re
def recv_until(s, marker, timeout=3.0):
    s.settimeout(timeout); buf=b""
    try:
        while marker not in buf:
            d=s.recv(4096)
            if not d: break
            buf+=d
    except socket.timeout: pass
    return buf
# 1) 배너 수신 → &secret_key 파싱
# 2) Mode 1 → %p 탐지로 버퍼 슬롯 확인
# 3) Mode 1 → "%<k>$s" + 패딩 + p64(addr) 로 키 유출
# 4) Mode 3 → 복원한 키를 hex로 제출
```

---

## 6. Level 3 Hidden — `fmt_bank` Mode 1 → Mode 2 (원격, 채점 · 심화)

- 목표: 전역 `is_vip_manager` 를 정확히 `0x1337beef` 로 만든 뒤 Mode 2 진입.
- `0x1337beef` 를 `%n` 한 번으로 쓰면 약 3.2억 글자를 출력해야 해 비현실적 → **상·하위 2바이트로 나눠 `%hn` 2회**:
  - `&is_vip_manager + 2` 에 상위 2바이트, `&is_vip_manager` 에 하위 2바이트.
  - 각 조각에서 **출력해야 할 누적 글자 수**를 계산해 `%<n>c` 로 맞춥니다(작은 값부터 쓰면 계산이 쉬움).
- 성공 시 `FLAG{fsb_arbitrary_write_got_overwrite_wizard_<학번>_<해시>}`.

접근 스캐폴드(직접 완성):
```python
# 주소 2개(vip+2, vip)를 8의 배수 경계에 배치하고
# "%<a>c%<k1>$hn%<b>c%<k2>$hn" 형태로 조각별 글자 수(a,b)와 슬롯(k1,k2)을 계산
```

---

## 7. GDB / 분석 빠른 참조

```bash
# 오프셋(슬롯) 탐지: 마커 + %p 나열
printf 'AAAAAAAA.%6$p.%7$p.%8$p.%9$p.%10$p\n' | ./fmt_bank   # 로컬 바이너리로 감 잡기
# 전역 변수 주소 확인
objdump -t fmt_bank | grep -E 'secret_vault_key|is_vip_manager'
gdb ./fmt_bank        # b handle_feedback; run; x/40gx $rsp  로 스택 배치 관찰
```

---

## 8. 보고서 & 채점

- **제출물**: 본인이 작성한 익스플로잇(`solve_main` / `solve_hidden`)과 취약점 분석 보고서.
- **배점**: Level 3 Main 60% / Hidden 40% (Level 1·2는 참여 점수).
- **보고서 필수 항목**:
  1. x86_64 인자 매핑과 `%p` 탐지로 도출한 **버퍼 슬롯 오프셋 근거**
  2. `%s` 임의 읽기에서 주소 배치·슬롯 지정 논리
  3. `%hn` 분할 쓰기의 **조각별 출력 글자 수 계산 과정**(수식)

---

## 9. 자주 겪는 문제

| 증상 | 원인 / 해결 |
|------|-------------|
| 로컬 빌드 결과가 교안과 다름 | arm64/Mach-O로 빌드됨 → `file` 로 확인, amd64 환경(0장) 사용 |
| `%p` 로 secret이 안 보임 | 개수 부족 → `%p.` 를 20개 이상으로 늘리기(read 63B 한도 내) |
| `%s` 에서 세그폴트 | 슬롯 번호(`k`)나 주소 배치 오프셋이 틀림 → `%p` 탐지 재확인 |
| `%hn` 썼는데 값이 안 맞음 | 조각별 누적 글자 수 계산 오류 → 작은 값부터 쓰고 차이만 추가 출력 |
| 원격 응답이 잘림 | 단일 `recv` 대신 `recv_until()` 로 프롬프트까지 누적 수신 |
| 포트 충돌(9003) | 기존 컨테이너 종료: `docker compose down` 후 재기동 |

---

## 10. 참고 자료 (링크)

- pwnable.kr — `fsb` (Format String Bug 입문 문제)
- CTF Wiki — Format String Vulnerability: https://ctf-wiki.org/pwn/linux/user-mode/fmtstr/fmtstr-intro/
- man 3 printf — 포맷 지정자 레퍼런스 (`%n`, `%hn`, 폭 지정 `%Nc`, 위치 인자 `%k$`)
- Phrack #59-0x07 "Format String Attacks" (역사적 원전)
- LiveOverflow — Format String Vulnerabilities (YouTube 시리즈)
