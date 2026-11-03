# READ ME FIRST — 10주차 디버깅 기초 & 보안 기법 관찰

## 개요
9주차에 구성한 **QEMU/OP-TEE 공용 환경**(`optee_qemu_env/`)을 그대로 사용합니다. 새로 내려받을 파일은 없습니다.
OP-TEE TA의 구조, gdb를 이용한 커널 추적, 시스템 레지스터, KASLR을 직접 확인합니다.

## 준비
- 9주차 환경(`optee_qemu_env/`)과 호스트 gdb(macOS `aarch64-elf-gdb`, Linux/WSL2 `gdb-multiarch`)가 필요합니다.
- 9주차 환경이 없다면 [`week09_optee/`](../week09_optee)의 안내에 따라 먼저 구성합니다.

## 진행
`LAB_GUIDE.md`의 절차를 순서대로 따릅니다. 각 단계에 예상 출력이 함께 적혀 있습니다.

## 제출 (보고서형)
- 스크린샷 3장을 하나의 PDF로 묶고, 화면마다 지정된 내용을 1~2줄로 설명합니다.
  1. TA 목록 + hello 예제 출력 + Secure World 로그
  2. gdb에서 `start_kernel` → `console_init` → `rest_init` 순서로 정지한 화면
  3. 두 번 부팅한 `_text` 값 + `SCTLR_EL1` 값
- 파일명 `[SSS]week10_학번_이름.pdf`, 마감은 LMS 공지 기준입니다.

## 채점 무결성
- 이 주차는 flag가 없는 **보고서형**입니다. 캡처는 **본인 환경에서 직접 실행한** 화면이어야 합니다.
- `_text` 주소는 부팅마다 달라지므로, 가이드의 예시 값이 아닌 본인이 얻은 값을 제출합니다.

## 문의
TA 임우진 · 공학2호관 633호 · woojinim64@o.cnu.ac.kr
메일 제목: `[소프트웨어및시스템보안][10주차_학번_이름] 문의내용`
