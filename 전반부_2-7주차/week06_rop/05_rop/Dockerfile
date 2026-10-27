FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y gcc make socat && rm -rf /var/lib/apt/lists/*
RUN useradd -m -s /bin/bash pwn
WORKDIR /tmp/build
COPY src/ /tmp/build/
RUN make clean && make && cp rop_vault /home/pwn/rop_vault && chmod 755 /home/pwn/rop_vault && chown pwn:pwn /home/pwn/rop_vault && rm -rf /tmp/build
COPY src/rop_vault.c /home/pwn/rop_vault.c
RUN chmod 444 /home/pwn/rop_vault.c
COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
WORKDIR /home/pwn
EXPOSE 9001
ENTRYPOINT ["/entrypoint.sh"]
