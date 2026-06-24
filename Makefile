CC = gcc

TARGET = fripon 

SRC = $(wildcard src/**/*.c src/*.c)

RAYLIB = -lraylib -lm

CFLAGS = -Wall -Wextra -Isrc 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(RAYLIB)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
