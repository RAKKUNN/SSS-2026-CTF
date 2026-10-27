CC ?= gcc
CFLAGS = -Wall -O0 -fno-stack-protector -no-pie -z noexecstack
BIN = $(basename $(wildcard *.c))
all: $(BIN)
%: %.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f $(BIN)
