CC ?= gcc
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS = -Wall -O0 -fno-stack-protector -no-pie -Wno-format-security
else
    CFLAGS = -Wall -O0 -fno-stack-protector -Wno-format-security
endif

all: fmt_bank

fmt_bank: fmt_bank.c
	$(CC) $(CFLAGS) -o fmt_bank fmt_bank.c

clean:
	rm -f fmt_bank
