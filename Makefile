ECC = emcc
CC = cc
CFLAGS = -g -Wall -pedantic -lm
CLIBS = `sdl2-config --cflags --libs` -lSDL2_ttf

CFLAGSECC = -s USE_SDL=2 -s USE_SDL_TTF=2 --preload-file fonts -s WASM=1 -s STACK_SIZE=850000

SRC = main.c
TARGETECC = index.html
TARGET = snake

all: $(TARGET)  

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CLIBS) $(CFLAGS)

wasm: $(TARGETECC)

$(TARGETECC): $(SRC)
	$(ECC) $(SRC) $(CFLAGSECC) -o $(TARGETECC) $(CFLAGS)

clean:
	rm $(TARGET)

cleanwasm:
	rm $(TARGETECC)  
