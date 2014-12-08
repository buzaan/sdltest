CC=clang
CFLAGS=`sdl2-config --cflags` -Wall -std=c99
#LDFLAGS=-F/Users/buzaan/Library/Frameworks -framework SDL2
LDFLAGS=`sdl2-config --libs`
BUILD_DIR=build
SRC_DIR=src
OBJ=$(BUILD_DIR)/main.o $(BUILD_DIR)/scene.o $(BUILD_DIR)/sdltest.o


all: sdltest

main.o:sdltest.h
scene.o:sdltest.h
sdltest.o:sdltest.h scene.h

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< 

sdltest: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o sdltest

clean:
	rm $(BUILD_DIR)/*
