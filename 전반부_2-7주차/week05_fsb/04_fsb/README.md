# Track 04: Format String Bug (`fmt_bank`)

## 1. 문제 정보 (Challenge Info)
- **분야**: 포맷 스트링 버그 (Format String Bug - Arbitrary Read & Write)
- **난이도**: `pwnable.kr` fsb 급 (중/상)
- **접속 정보**: `nc <HOST> 9003`
- **보호 기법**:
  - Canary: **Disabled** (`-fno-stack-protector`)
  - PIE: **Disabled** (`-no-pie`)
  - RELRO: Partial RELRO
- **제공 파일**: `src/fmt_bank.c` (소스코드) 및 컨테이너 바이너리

> 📘 단계별 실습 방법·환경 설정·GDB·접근법은 상위 폴더의 [`LAB_GUIDE.md`](../LAB_GUIDE.md) 를 먼저 읽으세요.

---

## 2. 시나리오 및 목표 (Scenario & Objectives)
`fmt_bank`는 고객 메모 처리 루틴에서 사용자 입력을 검증 없이 `printf(memo)`로 직접 출력하는 취약점이 존재합니다. 임의 메모리 읽기(Arbitrary Read)와 쓰기(Arbitrary Write)를 수행해 2개의 플래그를 획득하세요.

1. **Main Flag — Arbitrary Read via `%s` (Mode 1 → Mode 3)**
   - 배너가 출력하는 `&secret_vault_key` 주소를 확인하고, Mode 1(피드백)의 포맷 스트링으로 그 주소를 **역참조(`%s`)** 하여 저장된 64비트 비밀 키를 유출합니다.
   - 유출한 키를 Mode 3(Unlock Vault)에 입력하면 메인 플래그를 얻습니다. (`/home/pwn/flag_main.txt`)
2. **Hidden Flag — Arbitrary Write via `%n`/`%hn` (Mode 1 → Mode 2)**
   - Mode 2(VIP 대시보드)에 진입하려면 전역 변수 `is_vip_manager` 를 정확히 `0x1337beef` 로 만들어야 합니다.
   - Mode 1의 포맷 스트링으로 `is_vip_manager` 주소에 값을 **써넣어**(`%n`/`%hn`) 조건을 만족시키고 히든 플래그를 얻습니다. (`/home/pwn/flag_hidden.txt`)

---

## 3. 분석 힌트 (소스·배너·GDB로 직접 확인)

- **인자 매핑**: x86_64 System V ABI에서 `printf`의 가변 인자 1~5번은 레지스터(`rsi, rdx, rcx, r8, r9`), **6번째부터 스택**에 놓입니다. `memo` 버퍼가 스택의 몇 번째 인자 슬롯(`%k$`)에 대응하는지는 **`%p`를 연달아 보내 직접 탐지**하세요(입력한 마커가 어느 슬롯에 나타나는지 관찰).
- **임의 읽기(Main)**: `%k$s` 로 특정 슬롯의 값을 **주소로 보고 문자열 역참조**합니다. 유출할 주소를 페이로드 안에 8바이트로 배치한 뒤, 그 주소가 놓인 슬롯 번호를 `%k$s` 의 `k` 로 지정합니다. (오프셋 값은 위 탐지 결과로 산정)
  - ⚠️ 유출된 문자열은 **리틀엔디언 바이트 순서**로 화면에 찍힙니다(값이 뒤집혀 보임). 유출 8바이트를 `struct.unpack("<Q", raw)` 로 정수로 되돌려 제출하세요.
- **임의 쓰기(Hidden)**: `%n` 은 지금까지 출력된 문자 수를 4바이트로 기록, `%hn` 은 2바이트로 기록합니다. `0x1337beef` 를 한 번에 쓰면 3.2억 자를 출력해야 하므로, **상·하위 2바이트로 나눠 `%hn` 2회**로 쓰는 편이 현실적입니다(각 조각의 출력 글자 수 계산이 핵심).
- 완성 페이로드·정확한 오프셋 값은 제공하지 않습니다. 배너 주소·`gdb`·`%p` 탐지로 여러분이 직접 도출하세요.

---

## 4. LLM 활용 팁 (개념 이해용)
> AI는 **개념 이해**에만 쓰고 완성 익스플로잇을 그대로 제출하지 마세요(부정행위). 보고서에는 본인이 도출한 오프셋 근거와 글자 수 계산 과정이 있어야 합니다.

```markdown
[개념 질문 예시 1] "x86_64 리눅스에서 printf(buf) 포맷 스트링 취약점일 때, 입력 버퍼가 몇 번째 가변 인자 오프셋(%k$)에 대응하는지 %p로 찾는 원리를 설명해줘."
[개념 질문 예시 2] "%hn 2바이트 분할 쓰기로 특정 주소에 임의의 32비트 값을 기록할 때, 각 조각에서 출력해야 하는 문자 수를 계산하는 원리만 설명해줘."
```
