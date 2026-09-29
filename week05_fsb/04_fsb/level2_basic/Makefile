CC = gcc
CFLAGS = -Wall -O0 -fno-stack-protector -no-pie -Wno-format-security

all: fsb_basic

fsb_basic: fsb_basic.c
	$(CC) $(CFLAGS) -o fsb_basic fsb_basic.c

clean:
	rm -f fsb_basic
