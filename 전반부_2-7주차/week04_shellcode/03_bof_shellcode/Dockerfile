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
    cp echo_sandbox /home/pwn/echo_sandbox && \
    chmod 755 /home/pwn/echo_sandbox && \
    chown pwn:pwn /home/pwn/echo_sandbox && \
    rm -rf /tmp/build

COPY src/*.c /home/pwn/
RUN chmod 444 /home/pwn/*.c

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /home/pwn

EXPOSE 9002

ENTRYPOINT ["/entrypoint.sh"]
