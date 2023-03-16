CC := clang
CFLAGS := -Wall -pedantic -std=c17

OBJS := main.o \
input\input.o  \
field\field.o  \
saveio\saveio.o

all: CFLAGS += -D_PRETTY=1
all: $(OBJS) build

release: CFLAGS += -O3 -D_PRETTY=1
release: $(OBJS) build

nonansi: $(OBJS) build

nonansi_release: CFLAGS += -O3
nonansi_release: $(OBJS) build

build:
	$(CC) $(CFLAGS) -o minesweeper.exe $(OBJS)
	@echo $(CFLAGS)

$(OBJS): clean
$(OBJS):%.o : %.c

clean:
	del $(OBJS)