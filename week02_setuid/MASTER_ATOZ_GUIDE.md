# [소프트웨어및시스템보안] CTF 4종 문제 A to Z 마스터 가이드북 (Master Guide)

본 문서는 **소프트웨어및시스템보안** 교과목의 시스템 해킹(Pwnable) 4대 핵심 트랙에 대한 **이론, 메모리 구조, 취약점 분석, 단계별 익스플로잇(Main/Hidden), LLM 협업 프롬프트, 보안 패치**를 집대성한 종합 가이드입니다.

---

## ⚠️ 채점 무결성 규칙 (Grading Integrity — 필독)

> CTF 컨테이너는 학생이 **본인 PC에서 직접 구동**하므로, 파일 권한만으로 플래그를 숨길 수 없습니다.
> (호스트 관리자 권한으로 `docker exec -u 0`, `docker cp` 등이 항상 가능) 따라서 채점의 기준은
> "플래그 파일을 읽었는가"가 아니라 **"취약점을 직접 익스플로잇했는가"** 입니다.

1. **진입은 SSH guest 전용** — `ssh guest@localhost -p 2222` (PW: guest). `docker exec`(=root) 진입은 실습·채점 무효(0점). 진입 후 `id` 로 `uid=1000(guest)` 확인.
2. **플래그 문자열만 제출 = 0점** — Level 3 Main/Hidden은 **익스플로잇 코드 + 취약점 분석 보고서(GDB 덤프·오프셋·원리)** 로 채점. 플래그는 본인 확인용 토큰.
3. **본인 학번 플래그만 유효** — 타인 플래그·오프라인 계산 값 제출은 부정행위 로그 기록 → 0점 및 학사 조치.
4. **Level 1/2 는 로컬 연습(참여 점수)** — 특히 `suid_cat` 은 임의 파일 root 리더이므로 채점 플래그에 사용 금지.

---

## 📌 트랙별 핵심 개요 및 비교 매트릭스

| 트랙 | 대상 바이너리 | 주요 취약점 | Main Flag (기본 60%) | Hidden Flag (심화 40%) | 학습 핵심 개념 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **01. Set-UID** | `secure_logger` / `hidden_validator` | `system()` 상대 경로 호출 & `setresuid()` | PATH 환경변수 조작 (`PATH=/tmp:$PATH`) | `pipe()`, `dup2(3)`, `execve()` FD/환경변수 주입 런처 | Linux UIDs, `dash` 권한 드롭, IPC |
| **02. BOF** | `vault_guard` | `read()` 길이 미검증 스택 오버플로우 | 스택 인접 변수 (`auth_token`, `target_role`) 조작 | 64-bit ROP 체인 (`pop rdi; pop rsi; ret`) 인자 세팅 | Stack Frame Layout, x86_64 ABI |
| **03. Shellcode** | `echo_sandbox` | Executable Stack (`-z execstack`) | 스택 릭 기반 24B `execve` 쉘코드 실행 | Null-Free 비트반전(`not`) & XOR 자체 복호화 스텁 | Syscall ABI, Polymorphic Shellcode |
| **04. FSB** | `fmt_bank` | `printf(memo)` 직접 호출 | `%7$s` Direct Access 전역 비밀키 메모리 읽기 | `%hn` 2바이트 분할 쓰기 (`0x1337beef` 변조) | Variadic Funcs, Format Specifiers, Split Write |

---

## 🤖 LLM 활용 페어 프로그래밍 마스터 전략

시스템 해킹에서 LLM(대형 언어 모델)을 효과적으로 활용하려면 **점진적 질의(Stepwise Prompting)** 기법을 사용해야 합니다:

```
┌────────────────────────────────────────────────────────┐
│ 1단계: 취약점 발견 및 원리 설명 요청                    │
│   "소스코드와 checksec 결과를 제공하고 취약점 지점 분석" │
├────────────────────────────────────────────────────────┤
│ 2단계: 수학적 오프셋 및 제약 계산 요청                  │
│   "스택 레이아웃, 레지스터 호출 규약, 문자 수 계산식 유도"│
├────────────────────────────────────────────────────────┤
│ 3단계: 익스플로잇 코드 합성 및 디버깅                   │
│   "pwntools 페이로드 생성 및 gdb 출력 피드백 기반 미세조정"│
└────────────────────────────────────────────────────────┘
```

상세 프롬프트 템플릿과 예시는 [LLM_PROMPTING_GUIDE.md](LLM_PROMPTING_GUIDE.md)에 정리되어 있습니다.

---

## 🛡️ 시스템 보안 3대 방어 원칙 요약 (Defensive Cheat-Sheet)

1. **메모리 보호 기법(Mitigations) 활성화**:
   - **No-eXecute (NX/DEP)**: 스택/힙 실행 권한 박탈 (`-z noexecstack`)
   - **Stack Canary**: 스택 버퍼 오버플로우 탐지 (`-fstack-protector-all`)
   - **PIE / ASLR**: 코드/라이브러리/스택 주소 무작위화 (`-fPIE -pie`, `randomize_va_space=2`)
   - **Full RELRO**: GOT 덮어쓰기 방지 (`-Wl,-z,relro,-z,now`)
2. **시큐어 코딩(Secure Coding)**:
   - 바운드 체크가 없는 함수(`gets`, `strcpy`, `scanf("%s")`) 대신 길이 제한 함수(`fgets`, `read`, `snprintf`) 사용
   - 포맷 스트링 지정자 필수 명시 (`printf("%s", str);`)
3. **최소 권한의 원칙(Principle of Least Privilege)**:
   - SUID 바이너리는 절대 경로 명시 및 쉘을 거치지 않는 `execve` 호출
   - 특권 작업 완료 후 즉시 일반 사용자로 영구 권한 강하
