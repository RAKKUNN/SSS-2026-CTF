CC = gcc
CFLAGS = -Wall -O2

all: suid_cat

suid_cat: suid_cat.c
	$(CC) $(CFLAGS) -o suid_cat suid_cat.c

clean:
	rm -f suid_cat
