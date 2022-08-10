CC := gcc
CFLAGS := -g -Wall -pedantic

OBJS := main.o \
input\input.o \
field\field.o

all: CFLAGS := $(CFLAGS) -D_PRETTY=1
all: $(OBJS) build
#$(CC) $(CFLAGS) -o minesweeper $(OBJS)

release: CFLAGS := $(CFLAGS) -O3
release: all
#$(CC) $(CFLAGS) -o minesweeper $(OBJS)

nonansi: $(OBJS) build

nonansi_release: $(CFLAGS) := -O3
nonansi_release: $(OBJS) build

build:
	$(CC) $(CFLAGS) -o minesweeper $(OBJS)

$(OBJS): %.o : %.c

clean:
	del $(OBJS)