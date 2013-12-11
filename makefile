CC=gcc
CFLAGS=-g -Wall -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
LFLAGS=-lfuse -lcrypto
SOURCE=lfs.c util.c
OBJECTS=$(SOURCE:.c=.o)
EXECUTABLE=lfs

all:$(SOURCE) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LFLAGS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -rf *.o $(EXECUTABLE)

