CC=clang
CFLAGS=`guile-config compile` `sdl2-config --cflags`  -Wall
LDFLAGS=`guile-config link` -F/Users/buzaan/Library/Frameworks -framework SDL2
#LDFLAGS=`guile-config link` `sdl2-config --libs`
BUILD_DIR=build
SRC_DIR=src
OBJ=$(BUILD_DIR)/main.o


all: sdltest

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< 

sdltest: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o sdltest
