CC := gcc
CFLAGS := -g -Wall -pedantic

OBJS := main.o \
input\input.o \
field\field.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o minesweeper $(OBJS)

$(OBJS): %.o : %.c

clean:
	del $(OBJS)