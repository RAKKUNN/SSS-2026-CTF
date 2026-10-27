CC = gcc
CFLAGS = -Wall -O0 -fno-stack-protector -Wno-format-security

all: fsb_intro

fsb_intro: fsb_intro.c
	$(CC) $(CFLAGS) -o fsb_intro fsb_intro.c

clean:
	rm -f fsb_intro
