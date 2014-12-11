CC=clang
CFLAGS=-Wall -Werror -std=c99 -fstrict-aliasing -mmacosx-version-min=10.6
LDLIBS=-framework SDL2

BUILD_DIR=build
SRC_DIR=src
OBJ=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*c))
DEPFILES=$(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(OBJ))

all: sdltest

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< 

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -MQ $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$<) -MM $< -o $@

$(BUILD_DIR):
	mkdir $@

sdltest: $(BUILD_DIR) $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJ) -o sdltest

.PHONY: clean
clean:
	rm $(BUILD_DIR)/*o
	rm $(BUILD_DIR)/*d

-include $(DEPFILES)
