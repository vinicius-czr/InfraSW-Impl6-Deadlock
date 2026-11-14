CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRCS = main.c commands.c
OBJS = $(SRCS:.c=.o)
TARGET = banker

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)