#!/bin/bash
SALT="SYS_SEC_2026_DEEP_EVAL"
SID="${STUDENT_ID:-default}"

make_flag() {
    local tag="$1"
    local raw="${SALT}_${tag}_${SID}"
    local h=$(echo -n "$raw" | sha256sum | cut -c1-8)
    echo "FLAG{${tag}_${SID}_${h}}"
}

echo "$(make_flag fsb_arbitrary_memory_leak_secret_revealed)" > /home/pwn/flag_main.txt
echo "$(make_flag fsb_arbitrary_write_got_overwrite_wizard)" > /home/pwn/flag_hidden.txt

chmod 444 /home/pwn/flag_main.txt /home/pwn/flag_hidden.txt
chown pwn:pwn /home/pwn/flag_main.txt /home/pwn/flag_hidden.txt

exec socat TCP-LISTEN:9003,reuseaddr,fork EXEC:/home/pwn/fmt_bank,stderr
