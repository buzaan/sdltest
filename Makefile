CC=clang
CFLAGS=-Wall -Werror -std=c99 -fstrict-aliasing -mmacosx-version-min=10.6
LDLIBS=-framework SDL2

BUILD_DIR=build
SRC_DIR=src
OBJ=$(BUILD_DIR)/main.o $(BUILD_DIR)/scene.o $(BUILD_DIR)/game.o $(BUILD_DIR)/scenes.o


all: sdltest

$(BUILD_DIR)/main.o:$(SRC_DIR)/sdltest.h
$(BUILD_DIR)/scene.o:$(SRC_DIR)/sdltest.h
$(BUILD_DIR)/scenes.o:$(SRC_DIR)/sdltest.h $(SRC_DIR)/scene.h $(SRC_DIR)/scenes.h
$(BUILD_DIR)/game.o:$(SRC_DIR)/sdltest.h $(SRC_DIR)/scene.h

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< 

sdltest: $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJ) -o sdltest

clean:
	rm $(BUILD_DIR)/*o
