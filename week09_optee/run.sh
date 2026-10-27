#!/usr/bin/env bash
# =============================================================================
# OP-TEE QEMUv8 실행 러너 — 9주차 실습 환경 (소프트웨어및시스템보안 2026-2)
# 호스트의 qemu-system-aarch64 사용 (TCG, host 무관: mac/Linux/WSL2 동일)
#
#   ./run.sh           OP-TEE(TrustZone) 모드
#                      - TF-A→OP-TEE→U-Boot→Linux 전체 부팅, Secure World 구동
#                      - Normal World 콘솔 = 이 터미널 / Secure World 로그 = image/secure_world.log
#                      - optee_example_hello_world 등 TEE 실습용 (9주차 소개 · 14주차 TA)
#
#   ./run.sh --debug   커널 디버깅 모드 (순수 Linux, OP-TEE 미포함)
#                      - QEMU가 :1234 에서 '정지(-S)'로 대기 → gdb 로 커널을 리셋부터 추적
#                      - 다른 터미널:  gdb-multiarch image/vmlinux        (mac: aarch64-elf-gdb)
#                                      (gdb) target remote :1234
#                                      (gdb) break start_kernel
#                                      (gdb) continue
#                      - 10·13주차 커널 디버깅의 기본 모드
#
#   종료: 콘솔에서 Ctrl-A 누른 뒤 X
# =============================================================================
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
IMG="$HERE/image"
QEMU="${QEMU:-qemu-system-aarch64}"

if ! command -v "$QEMU" >/dev/null 2>&1; then
    echo "[run.sh] '$QEMU' 를 찾을 수 없습니다. INSTALL.md 의 OS별 설치 안내를 참고하세요." >&2
    exit 1
fi

need() { for f in "$@"; do [ -e "$IMG/$f" ] || { echo "[run.sh] image/$f 누락 — 배포본을 다시 받아 풀어주세요." >&2; exit 1; }; done; }

MODE=optee
[ "${1:-}" = "--debug" ] && MODE=kdebug

if [ "$MODE" = "kdebug" ]; then
    # 커널 디버깅: 순수 virt + -S(리셋 정지) + gdbstub(:1234). OP-TEE/보안부팅 없음 →
    # gdb 브레이크포인트가 빠르고 신뢰성 있게 동작(nokaslr 로 심볼 주소 고정).
    need Image rootfs.cpio.gz vmlinux
    echo "[run.sh] 커널 디버깅 모드 — 순수 Linux(OP-TEE 미포함). QEMU가 :1234 에서 정지 대기."
    echo "         다른 터미널에서 (mac 은 aarch64-elf-gdb):"
    echo "           gdb-multiarch '$IMG/vmlinux' -ex 'target remote :1234' -ex 'break start_kernel' -ex continue"
    echo "         (종료: Ctrl-A 후 X)"
    cd "$IMG"
    exec "$QEMU" -display none -smp 1 -cpu cortex-a57 -m 1024 \
        -machine virt -kernel Image -initrd rootfs.cpio.gz \
        -append "console=ttyAMA0 nokaslr" \
        -serial mon:stdio -s -S
fi

# OP-TEE(TrustZone) 모드 — BL1 이 semihosting 으로 개별 bl*.bin 로드, BL33(U-Boot)가
# uImage+rootfs.cpio.uboot 로드. qemu_v8.mk run-only 인자와 동일.
need bl1.bin bl2.bin bl31.bin bl32.bin bl32_extra1.bin bl32_extra2.bin bl33.bin \
     uImage rootfs.cpio.uboot Image rootfs.cpio.gz
echo "[run.sh] OP-TEE(TrustZone) 부팅.  Secure World 로그:  tail -f '$IMG/secure_world.log'"
echo "         (종료: 이 터미널에서 Ctrl-A 누른 뒤 X)"
cd "$IMG"
exec "$QEMU" -display none -smp 2 -cpu max,pauth-impdef=on \
    -d unimp -semihosting-config enable=on,target=native -m 1057 \
    -machine "virt,acpi=off,secure=on,mte=off,gic-version=3,virtualization=false" \
    -bios bl1.bin -initrd rootfs.cpio.gz -kernel Image \
    -append "console=ttyAMA0,38400 keep_bootcon root=/dev/vda2" \
    -serial mon:stdio -serial "file:$IMG/secure_world.log"
