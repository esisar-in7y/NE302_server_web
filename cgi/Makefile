
CC = gcc
CFLAGS = -g -Wall -Wno-int-conversion -Wno-unused-parameter -Wno-unused-function -Wextra -std=c99 
EXEC=main
OBJS = $(SRCS:c=o)


run:
	$(CC) $(CFLAGS) main.c -o $(EXEC)
	./$(EXEC)