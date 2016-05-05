CC = gcc
CFLAGS = -g -Wall -Werror
LIB = -lpthread -lSDL -L/usr/include/SDL -L$(HOME)/local/lib
INCLUDE = -Ilibfractal/ -I$(HOME)/local/include -I$(HOME)/local/lib
SRC = main.c
TARGET = main

all: $(TARGET)

$(TARGET): main.o libfractal.a
	$(CC) $(CFLAGS) -o $(TARGET) main.o libfractal/libfractal.a $(LIB)

libfractal.a:
	make -C libfractal/

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(INCLUDE)

##############################################################################

.PHONY: clean

clean:
	rm -rf *o
	rm -rf main
	make clean -C libfractal/

###############################################################################	

.PHONY: lib

lib:
	make -C libfractal/
	
###############################################################################

.PHONY: test

test:
	make -C test/
