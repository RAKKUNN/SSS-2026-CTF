CC ?= gcc
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CFLAGS = -Wall -O0 -fno-stack-protector -no-pie -z execstack
else
    CFLAGS = -Wall -O0 -fno-stack-protector
endif

all: shellcode_basic

shellcode_basic: shellcode_basic.c
	$(CC) $(CFLAGS) -o shellcode_basic shellcode_basic.c
	@echo 'FLAG{shellcode_basic_practice}' > flag.txt

clean:
	rm -f shellcode_basic flag.txt
