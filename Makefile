CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11

TARGET = client

all: $(TARGET)

client: client.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET)
