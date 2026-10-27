CC ?= gcc
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS = -Wall -O0 -fno-stack-protector -no-pie -z noexecstack
else
    CFLAGS = -Wall -O0 -fno-stack-protector
endif

all: vault_guard

vault_guard: vault_guard.c
	$(CC) $(CFLAGS) -o vault_guard vault_guard.c

clean:
	rm -f vault_guard
