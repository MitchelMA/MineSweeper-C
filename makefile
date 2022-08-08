CC := gcc
CFLAGS := -g -Wall -pedantic

OBJS := main.o \
input\input.o \
field\field.o

release: CFLAGS := $(CFLAGS) -O3
release: $(OBJS)
	$(CC) $(CFLAGS) -o minesweeper $(OBJS)

all: $(OBJS)
	$(CC) $(CFLAGS) -o minesweeper $(OBJS)

$(OBJS): %.o : %.c

clean:
	del $(OBJS)