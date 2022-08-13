CC := gcc
CFLAGS := -Wall -pedantic -std=c11

OBJS := main.o \
input\input.o \
field\field.o \
saveio\saveio.o

all: CFLAGS += -ggdb -D_PRETTY=1
all: $(OBJS) build

release: CFLAGS += -g0 -O3 -D_PRETTY=1
release: $(OBJS) build

nonansi: CFLAGS += -ggdb
nonansi: $(OBJS) build

nonansi_release: CFLAGS += -g0 -O3
nonansi_release: $(OBJS) build

build:
	$(CC) $(CFLAGS) -o minesweeper $(OBJS)
	@echo $(CFLAGS)

$(OBJS): %.o : %.c

clean:
	del $(OBJS)