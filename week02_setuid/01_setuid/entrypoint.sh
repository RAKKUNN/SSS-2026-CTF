#!/bin/bash
SALT="SYS_SEC_2026_DEEP_EVAL"
SID="${STUDENT_ID:-default}"

make_flag() {
    local tag="$1"
    local raw="${SALT}_${tag}_${SID}"
    local h=$(echo -n "$raw" | sha256sum | cut -c1-8)
    echo "FLAG{${tag}_${SID}_${h}}"
}

echo "$(make_flag setuid_path_hijack_dash_privdrop_bypass)" > /root/flag_main.txt
echo "$(make_flag setuid_fd_leak_env_integrity_master)" > /root/flag_hidden.txt
echo "FLAG{setuid_intro_practice}" > /root/flag_intro.txt
echo "FLAG{setuid_basic_practice}" > /root/flag_basic.txt

chmod 400 /root/flag_main.txt /root/flag_hidden.txt /root/flag_intro.txt /root/flag_basic.txt
chown root:root /root/flag_main.txt /root/flag_hidden.txt /root/flag_intro.txt /root/flag_basic.txt

exec /usr/sbin/sshd -D
