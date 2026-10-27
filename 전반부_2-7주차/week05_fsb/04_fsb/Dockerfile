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
    cp fmt_bank /home/pwn/fmt_bank && \
    chmod 755 /home/pwn/fmt_bank && \
    chown pwn:pwn /home/pwn/fmt_bank && \
    rm -rf /tmp/build

COPY src/*.c /home/pwn/
RUN chmod 444 /home/pwn/*.c

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

WORKDIR /home/pwn

EXPOSE 9003

# 학번별 동적 플래그: entrypoint가 STUDENT_ID 기반으로 flag 파일을 생성한 뒤 socat 구동
# (01/02/03 트랙과 동일한 방식으로 통일)
ENTRYPOINT ["/entrypoint.sh"]
