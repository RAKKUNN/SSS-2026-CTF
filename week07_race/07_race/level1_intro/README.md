# Level 1 (연습) — TOCTOU 눈으로 보기

> 참여/개념 확인용 **로컬 연습**입니다. 채점 플래그(Main/Hidden)는 컨테이너(L3)에서 얻습니다.

## 목표
`access()`(확인)와 `fopen()`(사용) 사이의 시간 간격 동안 심볼릭 링크를 바꿔치기하면,
확인했던 파일과 실제로 쓰이는 파일이 **달라진다**는 것을 직접 관찰한다.

## 환경 (어느 OS든 동일하게)
리눅스/SEED VM이면 그대로. macOS(Apple Silicon)·윈도우면 amd64 컨테이너 한 줄로 통일:
```bash
docker run --rm -it --platform linux/amd64 -v "$PWD":/w -w /w gcc:11 bash
```

## 절차
```bash
gcc -o vulp_practice vulp_practice.c
touch target_file secret_prize
ln -sf target_file target        # target -> 내가 쓸 수 있는 파일 (access 통과)
./vulp_practice                  # "지금부터 10초!" 출력되면
# ── 다른 터미널에서 (10초 안에) ──
ln -sf secret_prize target       # 링크를 secret_prize 로 교체
# 다시 vulp_practice 터미널에 아무 문자열 입력 + Enter
cat secret_prize                 # 입력이 secret_prize 에 들어갔으면 성공
```

## 성공 조건 & 제출
`secret_prize` 에 내가 입력한 내용이 기록되면 TOCTOU 를 재현한 것이다.
- **연습 플래그(정적, 참여점)**: `FLAG{race_intro_practice}`
- 보고서에는 "왜 target_file 이 아니라 secret_prize 에 쓰였는가"를 TOCTOU 로 설명한다.

## 핵심
`access()` 는 **실제 uid** 기준으로 "너 이거 쓸 수 있어?"만 확인할 뿐, 그 뒤 `fopen()` 이
**같은 대상**을 연다는 보장이 없다. 이 틈이 Race Window 다.
