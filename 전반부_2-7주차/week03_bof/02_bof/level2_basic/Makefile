CC = gcc
CFLAGS = -Wall -O0 -fno-stack-protector -no-pie

all: bof_ret2win

bof_ret2win: bof_ret2win.c
	$(CC) $(CFLAGS) -o bof_ret2win bof_ret2win.c

clean:
	rm -f bof_ret2win
