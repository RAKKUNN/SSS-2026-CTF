#!/bin/bash
SALT="SYS_SEC_2026_DEEP_EVAL"
SID="${STUDENT_ID:-default}"

make_flag() {
    local tag="$1"
    local raw="${SALT}_${tag}_${SID}"
    local h=$(echo -n "$raw" | sha256sum | cut -c1-8)
    echo "FLAG{${tag}_${SID}_${h}}"
}

echo "$(make_flag shellcode_stack_exec_classic_pwn)" > /home/pwn/flag_main.txt
echo "$(make_flag shellcode_badchar_xor_encoder_orw_god)" > /home/pwn/flag_hidden.txt

chmod 444 /home/pwn/flag_main.txt /home/pwn/flag_hidden.txt
chown pwn:pwn /home/pwn/flag_main.txt /home/pwn/flag_hidden.txt

exec socat TCP-LISTEN:9002,reuseaddr,fork EXEC:/home/pwn/echo_sandbox,stderr
