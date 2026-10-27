FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    openssh-server gcc make gdb strace vim sudo openssl ca-certificates \
    && rm -rf /var/lib/apt/lists/*
RUN mkdir -p /run/sshd

# 저권한 guest (무 sudo)
RUN useradd -m -s /bin/bash guest && echo 'guest:guest' | chpasswd

# 취약 바이너리 빌드 + setuid-root (Main)
COPY src/vulp.c /tmp/vulp.c
RUN gcc -O0 -o /usr/local/bin/vulp /tmp/vulp.c \
    && chown root:root /usr/local/bin/vulp && chmod 4755 /usr/local/bin/vulp \
    && rm -f /tmp/vulp.c

# 취약 바이너리 빌드 + setuid-root (Hidden, 극소 윈도우)
COPY src/vulp_hidden.c /tmp/vulp_hidden.c
RUN gcc -O0 -o /usr/local/bin/vulp_hidden /tmp/vulp_hidden.c \
    && chown root:root /usr/local/bin/vulp_hidden && chmod 4755 /usr/local/bin/vulp_hidden \
    && rm -f /tmp/vulp_hidden.c

COPY src/setup_workdir.sh /usr/local/bin/setup_workdir.sh
RUN chmod 755 /usr/local/bin/setup_workdir.sh

COPY entrypoint.sh /entrypoint.sh
RUN chmod 700 /entrypoint.sh

# sshd: 비번 로그인 허용, root 로그인 불가(빈 비번 거부는 기본값 유지)
RUN sed -i 's/#\?PermitRootLogin.*/PermitRootLogin no/' /etc/ssh/sshd_config \
    && sed -i 's/#\?PasswordAuthentication.*/PasswordAuthentication yes/' /etc/ssh/sshd_config

EXPOSE 22
ENTRYPOINT ["/entrypoint.sh"]
