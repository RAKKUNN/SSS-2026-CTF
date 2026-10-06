#!/bin/bash
SALT="SYS_SEC_2026_DEEP_EVAL"
SID="${STUDENT_ID:-default}"
MODE="${MODE:-main}"

make_flag() {
    local tag="$1"
    local raw="${SALT}_${tag}_${SID}"
    local h=$(echo -n "$raw" | sha256sum | cut -c1-8)
    echo "FLAG{${tag}_${SID}_${h}}"
}

if [ "$MODE" = "hidden" ]; then
    make_flag rop_aslr_bypass_leak_hidden > /home/pwn/flag.txt
    ARG=hidden
else
    make_flag rop_ret2libc_main > /home/pwn/flag.txt
    ARG=main
fi
chmod 444 /home/pwn/flag.txt
chown pwn:pwn /home/pwn/flag.txt

exec socat TCP-LISTEN:9001,reuseaddr,fork EXEC:"/home/pwn/rop_vault $ARG",stderr
