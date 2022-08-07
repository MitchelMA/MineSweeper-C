CC := gcc
CLFALGS := -g -Wall -pedantic

OBJS := main.o

all: $(OBJS)
	$(CC) $(CLFALGS) -o main $(OBJS)

$(OBJS): %.o : %.c

clean:
	del $(OBJS)