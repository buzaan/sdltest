CC=clang
CFLAGS=-Wall -Werror -std=c99 -fstrict-aliasing -mmacosx-version-min=10.6
LDLIBS=-framework SDL2

BUILD_DIR=build
SRC_DIR=src
OBJ=$(BUILD_DIR)/main.o $(BUILD_DIR)/scene.o $(BUILD_DIR)/game.o $(BUILD_DIR)/menu_scene.o $(BUILD_DIR)/graphics.o $(BUILD_DIR)/input.o


all: sdltest

$(BUILD_DIR)/main.o:$(SRC_DIR)/sdltest.h $(SRC_DIR)/game.h $(SRC_DIR)/scene.h $(SRC_DIR)/menu_scene.h
$(BUILD_DIR)/scene.o:$(SRC_DIR)/sdltest.h
$(BUILD_DIR)/game.o:$(SRC_DIR)/sdltest.h $(SRC_DIR)/scene.h $(SRC_DIR)/game.h
$(BUILD_DIR)/menu_scene.o:$(SRC_DIR)/sdltest.h $(SRC_DIR)/scene.h $(SRC_DIR)/menu_scene.h
$(BUILD_DIR)/graphics.o:$(SRC_DIR)/graphics.h
$(BUILD_DIR)/input.o:$(SRC_DIR)/input.h $(SRC_DIR)/sdltest.h

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< 

$(BUILD_DIR):
	mkdir $@

sdltest: $(BUILD_DIR) $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJ) -o sdltest

clean:
	rm $(BUILD_DIR)/*o
