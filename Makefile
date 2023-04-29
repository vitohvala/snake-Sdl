ECC = emcc
CC = cc
CFLAGS = -g -Wall -pedantic -lm
CLIBS = `sdl2-config --cflags --libs`

CFLAGSECC = -s USE_SDL=2 -s WASM=1 -s STACK_SIZE=650000

SRC = src/main.c
TARGETECC = WASM/index.js
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
