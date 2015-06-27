CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -D_BSD_SOURCE

ifneq ($(RAZRI_IMAGE),)
	CFLAGS+=-DRAZRI_IMAGE=1
endif

.PHONY: clean

all: pack_intel unpack_intel

clean:
	rm -f *.o pack_intel unpack_intel pack_intel.exe unpack_intel.exe *~

pack_intel: pack.o
	$(CC) -o $@ $< $(CFLAGS)

unpack_intel: unpack.o
	$(CC) -o $@ $< $(CFLAGS)
