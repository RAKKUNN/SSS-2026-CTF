# [BOF Track] Level 2: 기본 (Basic) - `bof_ret2win`

## 1. 문제 개요
스택 버퍼 오버플로우를 통해 **함수의 리턴 어드레스(Return Address, RET)**를 덮어써서 실행되지 않는 `win()` 함수로 제어 흐름을 가로채는(Control Flow Hijacking) 표준 ret2win 문제입니다.

---

## 2. 취약점 분석
```c
void vulnerable_function() {
    char buffer[32];
    read(0, buffer, 128); // 버퍼 크기보다 큰 입력 허용
}
```
- 버퍼를 넘겨 입력하면 `Saved RBP`를 지나 **Return Address(RET)** 까지 덮을 수 있습니다.
- RET 자리에 `win()` 함수 주소를 놓으면 함수 리턴 시 `win()`이 호출됩니다.

---

## 3. 공략 방향 (직접 수행)
- **오프셋**: `버퍼 크기 + Saved RBP(8B)`. 정확한 값은 `gdb`의 `disas vulnerable_function`(버퍼 위치 확인) 으로 **직접 계산**하세요.
- **win() 주소**: 프로그램이 실행 시 출력하며, `gdb`의 `p win` 또는 `objdump -d ./bof_ret2win | grep '<win>:'` 로도 확인됩니다. (빌드마다 다를 수 있으니 하드코딩 금지)
- 위 오프셋과 주소로 페이로드를 **직접 구성**하세요. 완성 페이로드/명령은 제공하지 않습니다.

---

## 4. 플래그
- `win()`이 실행되면 성공 메시지와 함께 화면에 출력됩니다. (형식: `FLAG{...}`)
