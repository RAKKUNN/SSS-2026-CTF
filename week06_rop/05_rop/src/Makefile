CC ?= gcc
CFLAGS = -Wall -O0 -fno-stack-protector -no-pie -z noexecstack
all: rop_vault
rop_vault: rop_vault.c
	$(CC) $(CFLAGS) -o rop_vault rop_vault.c
clean:
	rm -f rop_vault
