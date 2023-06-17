CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -Werror
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = scc

scc: $(OBJS)
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

run: scc
	@./$(TARGET)

.PHONY: clean test
test:
	./$(TARGET) ../tests/syntax.c
clean:
	rm -f $(TARGET) $(OBJS) $(OBJS:.o=.d)
	rm -f *~
