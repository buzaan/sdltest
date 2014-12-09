CC=clang
CFLAGS=-Wall -std=c99 -fstrict-aliasing -mmacosx-version-min=10.6
LDLIBS=-framework SDL2

BUILD_DIR=build
SRC_DIR=src
OBJ=$(BUILD_DIR)/main.o $(BUILD_DIR)/scene.o $(BUILD_DIR)/sdltest.o $(BUILD_DIR)/scenes.o


all: sdltest

main.o:sdltest.h scenes.h
scene.o:sdltest.h
scenes.o:scene.h scenes.h
sdltest.o:sdltest.h scene.h

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< 

sdltest: $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJ) -o sdltest

clean:
	rm $(BUILD_DIR)/*o
