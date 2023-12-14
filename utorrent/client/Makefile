CC = g++
CFLAGS = -Wall -std=c++11
LDFLAGS = -lssl -lcrypto

SRC_DIR = .
OBJ_DIR = .
BIN_DIR = .

CLIENT_SRC = $(SRC_DIR)/client.cpp
UTILITY_SRC = $(SRC_DIR)/general_utility_functions.cpp
BLUEPRINTS_H = $(SRC_DIR)/blueprints.h

CLIENT_OBJ = $(OBJ_DIR)/client.o
UTILITY_OBJ = $(OBJ_DIR)/general_utility_functions.o

EXECUTABLE = $(BIN_DIR)/client

all: $(EXECUTABLE)

$(EXECUTABLE): $(CLIENT_OBJ) $(UTILITY_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(CLIENT_OBJ): $(CLIENT_SRC) $(BLUEPRINTS_H)
	$(CC) $(CFLAGS) -c -o $@ $<

$(UTILITY_OBJ): $(UTILITY_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXECUTABLE)

.PHONY: all clean
