# [BOF Track] Level 2: 기본 (Basic) - `bof_ret2win`

## 1. 문제 개요
스택 버퍼 오버플로우를 통해 **함수의 리턴 어드레스(Return Address, RET)**를 덮어써서 실행되지 않는 `win()` 함수로 제어 흐름을 가로채는(Control Flow Hijacking) 표준 ret2win 문제입니다.

---

## 2. 취약점 분석
```c
void vulnerable_function() {
    char buffer[32];
    read(0, buffer, 128); // 32바이트 버퍼에 128바이트 입력 허용
}
```
- `buffer`(32바이트) + `Saved RBP`(8바이트) = **총 40바이트 오프셋**.
- 40바이트 패딩 뒤에 `win()` 함수의 64비트 주소(`p64(win_addr)`)를 배치하면, `vulnerable_function`이 리턴하는 순간 `win()` 함수가 호출되어 플래그가 출력됩니다.

---

## 3. 공략법
```bash
python3 solve.py
```

---

## 4. 플래그
`FLAG{bof_basic_practice}`
