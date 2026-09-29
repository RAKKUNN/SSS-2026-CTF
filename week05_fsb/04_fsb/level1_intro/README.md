# [FSB Track] Level 1: 기초 (Intro) — `fsb_intro` (로컬 연습)

## 1. 문제 개요
포맷 스트링 버그(Format String Bug)의 기본인 **스택 메모리 정보 유출(Stack Memory Leak via `%p`)** 을 체험하는 입문 문제입니다. (참여 점수 · 채점 서버 제출 대상 아님)

## 2. 취약점 분석
```c
volatile uint64_t secret_token = 0x1337cafebeefdead;
char name[64];
read(0, name, sizeof(name) - 1);
printf(name); // Format String Vulnerability
```
- `printf` 에 형식 지정자 없이 `name` 버퍼를 직접 전달하므로, `%p` 를 연달아 입력하면 스택에 저장된 값들이 순서대로 출력됩니다.

## 3. 실행 (amd64 환경에서)
> ⚠️ 맥 네이티브(`gcc`)는 arm64/Mach-O가 만들어져 오프셋·동작이 교안과 달라집니다. SEED VM / WSL2 / `--platform linux/amd64` 컨테이너에서 빌드하세요 ([`LAB_GUIDE.md`](../../LAB_GUIDE.md) 0장).
```bash
make
./fsb_intro
```
- `%p` 를 **넉넉히**(예: `%p.` 를 20개 이상) 입력해 유출값 중 `0x1337cafebeefdead` 를 찾으세요. (`secret_token` 은 스택 상위 슬롯에 있어 몇 개만 보내면 안 보일 수 있습니다.)
- 찾은 값을 2단계 입력(0x 없이 hex)에 그대로 제출하면 플래그가 출력됩니다.

## 4. 플래그
`FLAG{fsb_intro_practice}`
