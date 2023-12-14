CC = g++
CFLAGS = -Wall -std=c++11
LDFLAGS =

SRC_DIR = .
OBJ_DIR = .
BIN_DIR = .

TRACKER_SRC = $(SRC_DIR)/tracker.cpp
UTILITY_SRC = $(SRC_DIR)/general_utility_functions.cpp
BLUEPRINTS_H = $(SRC_DIR)/blueprints.h

TRACKER_OBJ = $(OBJ_DIR)/tracker.o
UTILITY_OBJ = $(OBJ_DIR)/general_utility_functions.o

EXECUTABLE = $(BIN_DIR)/tracker

all: $(EXECUTABLE)

$(EXECUTABLE): $(TRACKER_OBJ) $(UTILITY_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TRACKER_OBJ): $(TRACKER_SRC) $(BLUEPRINTS_H)
	$(CC) $(CFLAGS) -c -o $@ $<

$(UTILITY_OBJ): $(UTILITY_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXECUTABLE)

.PHONY: all clean
