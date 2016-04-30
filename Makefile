all: main.c
	gcc -pthread main.c libfractal/libfractal.a $(pkg-config --cflags --libs sdl) -o main

