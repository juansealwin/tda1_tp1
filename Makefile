CFLAGS := -ggdb3 -O2 -Wall -Wextra -std=c11
CFLAGS += -Wmissing-prototypes -Wvla
CPPFLAGS := -D_DEFAULT_SOURCE

PROGS := tp1 

all: $(PROGS)

tp1: tp1.o

clean:
	rm -f $(PROGS) *.o

.PHONY: all clean
