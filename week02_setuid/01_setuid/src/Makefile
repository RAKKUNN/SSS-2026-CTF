CC = gcc
CFLAGS = -Wall -O2

all: secure_logger hidden_validator

secure_logger: secure_logger.c
	$(CC) $(CFLAGS) -o secure_logger secure_logger.c

hidden_validator: hidden_validator.c
	$(CC) $(CFLAGS) -o hidden_validator hidden_validator.c

clean:
	rm -f secure_logger hidden_validator
