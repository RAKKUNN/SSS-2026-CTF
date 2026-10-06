# 6주차 ROP — Level 1 (로컬 연습): 단일 가젯 ROP

NX가 켜진 바이너리에서 **RET을 덮어** `pop rdi; ret` 가젯으로 함수에 인자 1개를 전달하는 첫걸음.

## 빌드 (전 OS 동일 — amd64 컨테이너)
```bash
docker run --rm --platform linux/amd64 -v "$PWD":/w -w /w gcc:11 \
  gcc -O0 -fno-stack-protector -no-pie -z noexecstack -o rop_intro rop_intro.c
```

## 과제
1. 실행하면 `buf`, `unlock`, `gadget` 주소가 출력됩니다.
2. `objdump -d rop_intro | grep -A1 'pop .*rdi'` 로 `pop rdi; ret` 가젯 주소를 찾으세요.
3. 버퍼 오프셋(버퍼 시작 → 저장된 RET)을 구하세요. (`cyclic` 또는 소스 분석)
4. `unlock()`이 요구하는 키 값을 **레지스터 규약(RDI)**에 맞게 전달해 플래그를 출력하세요.

> 성공 시 `FLAG{rop_intro_practice}` 가 출력됩니다. (로컬 연습용 정적 플래그 — 채점 서버 제출 대상 아님)
