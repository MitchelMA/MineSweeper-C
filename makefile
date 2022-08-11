CC := gcc
CFLAGS := -g -Wall -pedantic

OBJS := main.o \
input\input.o \
field\field.o \
saveio\saveio.o

all: CFLAGS += -D_PRETTY=1
all: $(OBJS) build
	@echo $(CFLAGS)
#$(CC) $(CFLAGS) -o minesweeper $(OBJS)
release: CFLAGS += -O3 -D_PRETTY=1
release: $(OBJS) build

nonansi: $(OBJS) build

nonansi_release: CFLAGS := $(CFLAGS) -O3
nonansi_release: $(OBJS) build

build:
	$(CC) $(CFLAGS) -o minesweeper $(OBJS)

$(OBJS): %.o : %.c

clean:
	del $(OBJS)