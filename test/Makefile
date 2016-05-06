CC = gcc
CFLAGS = -g -Wall -Werror
LIB = -lpthread -lSDL -L/usr/include/SDL -L$(HOME)/local/lib -lcunit
INCLUDE = -Ilibfractal/ -I$(HOME)/local/include -I$(HOME)/local/lib
SRC = unitary_test.c
TARGET = test

all: $(TARGET)

$(TARGET): unitary_test.o libfractal.a
	$(CC) $(CFLAGS) -o $(TARGET) unitary_test.o ../libfractal/libfractal.a $(LIB)

libfractal.a:
	make -C ../libfractal/

unitary_test.o: unitary_test.c
	$(CC) $(CFLAGS) -c unitary_test.c $(INCLUDE)

################################################################################################

.PHONY: clean

clean:
	rm -rf *o
	rm -rf test


