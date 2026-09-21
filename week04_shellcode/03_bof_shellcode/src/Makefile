CC ?= gcc
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS = -Wall -O0 -fno-stack-protector -no-pie -z execstack
else
    CFLAGS = -Wall -O0 -fno-stack-protector
endif

all: echo_sandbox

echo_sandbox: echo_sandbox.c
	$(CC) $(CFLAGS) -o echo_sandbox echo_sandbox.c

clean:
	rm -f echo_sandbox
