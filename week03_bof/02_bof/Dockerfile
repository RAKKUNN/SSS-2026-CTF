FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    socat \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash pwn

WORKDIR /tmp/build
COPY src/ /tmp/build/
RUN make clean && make && \
    cp vault_guard /home/pwn/vault_guard && \
    chmod 755 /home/pwn/vault_guard && \
    chown pwn:pwn /home/pwn/vault_guard && \
    rm -rf /tmp/build

COPY src/*.c /home/pwn/
RUN chmod 444 /home/pwn/*.c

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /home/pwn

EXPOSE 9001

ENTRYPOINT ["/entrypoint.sh"]
