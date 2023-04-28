CC = gcc

CFLAGS = -g -Wall `sdl2-config --cflags --libs` -O3 -pedantic -lm
TARGET = snake

all: $(TARGET)
	$(CC) main.c -o $(TARGET)  $(CFLAGS) 
