# [Shellcode Track] Level 2: 기본 (Basic) — `shellcode_basic` (로컬 연습)

## 1. 문제 개요
스택 버퍼 오버플로우 + 쉘코드 주입을 결합해, 스택에 올린 쉘코드로 **복귀 주소(RET)** 를 바꿔 셸을 얻는 고전 Pwnable 표준 실습입니다. (참여 점수 · 채점 서버 제출 대상 아님)

## 2. 취약점 분석
```c
void vuln() {
    char buf[64];
    printf(" [*] Stack Buffer Address : %p\n", buf);
    read(0, buf, 128);      // 64B 버퍼에 128B 허용 -> RET 덮어쓰기
}
```
- `buf` 에 쉘코드를 넣고 남은 공간을 NOP(`\x90`)로 채운 뒤, **RET 위치**에 `buf` 시작 주소(little-endian `p64`)를 기록합니다.
- 오프셋은 `버퍼 크기 + Saved RBP(8)` 입니다. 정확한 값은 `gdb`의 `disas vuln`(버퍼 `lea -0x??(%rbp)` 위치)으로 **직접 계산**하세요.
- `vuln()` 리턴 시 스택의 쉘코드로 점프하여 실행됩니다.

## 3. 실행 (amd64 환경에서)
```bash
make            # shellcode_basic 빌드 + flag.txt 생성
./shellcode_basic
```
- 프로그램이 출력하는 버퍼 주소를 받아 페이로드를 **직접 구성**해 전달하세요(파이프 또는 pwntools). 성공 시 셸에서 `cat flag.txt`.

## 4. 플래그
`FLAG{shellcode_basic_practice}`  (성공 시 `flag.txt` 에서 확인)
