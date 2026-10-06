# 6주차 ROP — Level 2 (로컬 연습): 가젯 체이닝

함수를 하나만 호출하는 데서 나아가, 가젯으로 **여러 함수를 인자와 함께 연속 호출**합니다. (원본 3.3 실습2 대응)

## 빌드
```bash
docker run --rm --platform linux/amd64 -v "$PWD":/w -w /w gcc:11 \
  gcc -O0 -fno-stack-protector -no-pie -z noexecstack -o rop_basic rop_basic.c
```

## 과제
1. `stageA` → `stageB` **순서대로** 각자 올바른 인자와 함께 호출해야 합니다.
2. 각 함수 호출 앞에 `pop rdi; ret` 가젯을 두어 RDI를 세팅하세요.
3. 호출 순서/인자 중 하나라도 틀리면 실패합니다.

> 성공 시 `FLAG{rop_basic_practice}`. (로컬 연습용 정적 플래그 — 서버 제출 대상 아님)
> 심화(Main/Hidden)에서는 **in-binary 함수 대신 libc 함수(`system`)** 를 호출합니다(ret2libc).
