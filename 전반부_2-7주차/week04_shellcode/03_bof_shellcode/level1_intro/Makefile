CC ?= gcc
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS = -Wall -O0 -fno-stack-protector -z execstack
else
    CFLAGS = -Wall -O0 -fno-stack-protector
endif

all: shellcode_intro

shellcode_intro: shellcode_intro.c
	$(CC) $(CFLAGS) -o shellcode_intro shellcode_intro.c
	@echo 'FLAG{shellcode_intro_practice}' > flag.txt

clean:
	rm -f shellcode_intro flag.txt
