# READ ME FIRST — 9주차 OP-TEE QEMU 실습

## 이게 뭔가요
후반부(10~14주차) 임베디드·커널·부트로더 디버깅 실습을 위한 **공용 환경**입니다.
실물 보드 없이 각자 노트북에서 ARM TrustZone(Normal/Secure World)을 QEMU로 띄웁니다.

## 지금 할 일
1. `INSTALL.md` — OS별로 `qemu-system-aarch64`(+gdb) 설치.
2. `LAB_GUIDE.md` — 부팅 → OP-TEE 예제 → GDB 원격 접속, 단계별 절차와 제출물.

## 제출 (보고서형)
- 스크린샷 3장(①부팅+OP-TEE 배너 ②`TA incremented value to 43` ③gdb `start_kernel` 정지)
  을 하나의 PDF로 묶고, 각 단계 의미를 1~2줄 설명.
- 파일명 `[SSS]week09_학번_이름.pdf`, LMS 공지 마감.

## 채점 무결성
- 이 주차는 flag가 없는 **보고서형**입니다. 캡처는 **본인 환경에서 직접 실행한** 화면이어야 합니다.
- 환경 구성이 목적이니, 막히면 감추지 말고 `LAB_GUIDE.md` 트러블슈팅 → TA에게 문의하세요.

## 문의
TA 임우진 · 공학2호관 633호 · woojinim64@o.cnu.ac.kr
메일 제목: `[소프트웨어및시스템보안][9주차_학번_이름] 문의내용`
