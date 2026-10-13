#!/bin/bash
SALT="SYS_SEC_2026_DEEP_EVAL"
SID="${STUDENT_ID:-default}"

make_flag() {
    local tag="$1"
    local raw="${SALT}_${tag}_${SID}"
    local h=$(echo -n "$raw" | sha256sum | cut -c1-8)
    echo "FLAG{${tag}_${SID}_${h}}"
}

echo "$(make_flag race_toctou_symlink_passwd_main)"       > /root/flag_main.txt
echo "$(make_flag race_atomic_renameat2_exchange_hidden)" > /root/flag_hidden.txt
chmod 400 /root/flag_main.txt /root/flag_hidden.txt
chown root:root /root/flag_main.txt /root/flag_hidden.txt

# guest 작업 디렉토리 (매 기동 초기화)
/usr/local/bin/setup_workdir.sh

exec /usr/sbin/sshd -D
