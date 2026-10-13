# Level 2 (연습) — 루프로 레이스 자동화

> 참여/개념 확인용 **로컬 연습**입니다. 채점 플래그(Main/Hidden)는 컨테이너(L3)에서 얻습니다.

## 목표
L1처럼 손으로 10초 기다리지 않고, 심볼릭 링크를 **루프로 쉼 없이 번갈아** 걸어
짧은 윈도우에서도 레이스를 이기는 자동화 공격을 체험한다.

## 환경
L1과 동일. macOS/윈도우는 amd64 컨테이너 한 줄:
```bash
docker run --rm -it --platform linux/amd64 -v "$PWD":/w -w /w gcc:11 bash
```

## 절차
이 실습은 L1의 `vulp_practice` 와 여기 `attack_loop` **두 프로그램을 같은 작업 디렉토리**에서
함께 돌린다. L1 폴더의 `vulp_practice.c` 를 이 폴더로 가져와 빌드하자(윈도우를 `sleep(1)` 정도로
줄인 사본을 써도 좋다).
```bash
# 0) 두 프로그램을 같은 폴더에 준비
cp ../level1_intro/vulp_practice.c .     # L1 소스 가져오기
gcc -o vulp_practice vulp_practice.c     # (필요하면 sleep(10)→sleep(1) 로 줄여도 됨)
gcc -o attack_loop attack_loop.c

# 1) 대상 파일/링크 준비
touch target_file secret_prize
ln -sf target_file target

# 2) 링크 교체를 백그라운드로 돌리면서 취약 프로그램을 반복 실행
./attack_loop &                                  # 링크를 계속 번갈아 교체
while :; do echo "pwn" | ./vulp_practice; \
    grep -q pwn secret_prize && break; done      # secret_prize 가 바뀌면 승리
pkill attack_loop
cat secret_prize
```

## 성공 조건 & 제출
`secret_prize` 에 입력이 들어가면 성공.
- **연습 플래그(정적, 참여점)**: `FLAG{race_basic_practice}`
- 보고서에는 "루프 공격이 왜 수동보다 유리한가(윈도우가 짧아도 반복 시도로 확률↑)"를 설명한다.

## 핵심
레이스는 **확률 게임**이다. 시도 횟수를 늘리면 짧은 윈도우도 언젠가 맞는다.
다음 단계(L3 Hidden)에서는 윈도우가 더 짧아, **원자적(atomic)** 교체가 왜 필요한지 배운다.
