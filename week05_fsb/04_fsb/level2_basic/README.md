# [FSB Track] Level 2: 기본 (Basic) — `fsb_basic` (로컬 연습)

## 1. 문제 개요
포맷 스트링의 `%n` 지정자로 **임의 메모리 쓰기(Arbitrary Memory Write)** 를 수행해, 전역 변수 `target` 의 값을 `4` 로 조작하는 문제입니다. (참여 점수 · 채점 서버 제출 대상 아님)

## 2. 취약점 분석
- `%n` 은 지금까지 `printf` 가 출력한 총 문자(바이트) 수를 인자로 전달된 포인터 주소에 정수로 기록합니다.
- 정확히 **4글자를 출력**(`%4c`)한 뒤, 그 시점에 `&target` 이 놓인 인자 슬롯을 `%k$n` 으로 지정하면 `target` 에 `4` 가 기록됩니다.

## 3. 실행 (amd64 환경에서)
> ⚠️ SEED VM / WSL2 / `--platform linux/amd64` 컨테이너에서 빌드하세요 ([`LAB_GUIDE.md`](../../LAB_GUIDE.md) 0장).
```bash
make
./fsb_basic
```
- 배너에 출력되는 `&target` 주소를 확인하고, `%p` 탐지로 **입력 버퍼가 놓이는 인자 슬롯**을 찾으세요.
- `%4c` + `%k$n` + 정렬 패딩 + `p64(&target)` 형태로 페이로드를 구성합니다. (`k` 와 패딩 길이는 직접 도출)

## 4. 플래그
`FLAG{fsb_basic_practice}`
