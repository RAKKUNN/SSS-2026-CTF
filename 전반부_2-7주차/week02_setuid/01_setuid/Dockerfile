FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    openssh-server \
    gcc \
    make \
    sudo \
    python3 \
    gdb \
    curl \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Setup SSH Server
RUN mkdir /var/run/sshd
RUN echo 'root:root_pass_syslab2026!' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin no/' /etc/ssh/sshd_config
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# Create non-privileged student user: guest
RUN useradd -m -s /bin/bash guest && echo "guest:guest" | chpasswd

# Setup log file
RUN echo "[$(date)] SYSTEM_BOOT: Kernel initialized successfully." > /var/log/sysdiag.log && \
    echo "[$(date)] AUTH_DAEMON: Subsystem ready on port 22." >> /var/log/sysdiag.log && \
    chmod 644 /var/log/sysdiag.log

# Copy source code and build
WORKDIR /tmp/build
COPY src/ /tmp/build/
RUN make clean && make && \
    cp secure_logger /home/guest/secure_logger && \
    cp hidden_validator /home/guest/hidden_validator && \
    chown root:root /home/guest/secure_logger /home/guest/hidden_validator && \
    chmod 4755 /home/guest/secure_logger /home/guest/hidden_validator && \
    rm -rf /tmp/build

COPY src/*.c /home/guest/
RUN chown root:root /home/guest/*.c && chmod 444 /home/guest/*.c

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# --- 실습 위생: 진입 계정 안내/경고 -------------------------------------------
# docker exec(=root)로 들어오면 /root 플래그를 그냥 읽게 되므로, 이 경로가
# '실습/채점 대상이 아님'을 크게 경고한다. 정식 진입은 SSH guest 전용.
# (자가 호스팅 구조상 docker exec -u 0 를 '차단'할 수는 없으므로, 오용을
#  분명히 인지시키는 것이 목적. 실제 채점은 익스플로잇 코드+보고서로 한다.)
RUN printf '\n%s\n' \
'echo "=============================================================="' \
'echo " [!] 경고: 이 컨테이너에 ROOT 로 진입했습니다 (docker exec/root)."' \
'echo "     이 경로는 실습/채점 대상이 아닙니다 — /root 플래그를 그냥"' \
'echo "     읽는 것은 0점입니다. 반드시 아래로 접속해 직접 익스플로잇하세요:"' \
'echo "       ssh guest@localhost -p 2222   (PW: guest)"' \
'echo "=============================================================="' \
  >> /root/.bashrc
RUN printf '\n%s\n' \
'echo "[*] 계정: $(id -un) (uid=$(id -u)) | 목표: 권한상승으로 /root/flag_main.txt · /root/flag_hidden.txt 획득"' \
  >> /home/guest/.bashrc && chown guest:guest /home/guest/.bashrc

# SSH 접속 시 사전 배너 (guest 전용 안내)
RUN printf '%s\n' \
'=== [소프트웨어및시스템보안] Track 01: Set-UID (secure_logger) ===' \
'guest 계정 전용 서비스입니다. 목표: 권한 상승 취약점을 익스플로잇하여' \
'/root/flag_main.txt (Main) 와 /root/flag_hidden.txt (Hidden) 를 획득하세요.' \
  > /etc/ssh/ctf_banner && \
    sed -i 's|^#\?Banner .*|Banner /etc/ssh/ctf_banner|' /etc/ssh/sshd_config && \
    ( grep -q '^Banner ' /etc/ssh/sshd_config || echo 'Banner /etc/ssh/ctf_banner' >> /etc/ssh/sshd_config )
# -----------------------------------------------------------------------------

WORKDIR /home/guest

EXPOSE 22

ENTRYPOINT ["/entrypoint.sh"]
