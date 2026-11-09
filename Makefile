
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
TARGET = game_of_life

ifeq ($(OS),Windows_NT)
    TARGET = game_of_life.exe
    RM = del /f /q
    RUN = $(TARGET)
else
    RM = rm -f
    RUN = ./$(TARGET)
endif

SOURCES = src/main.c src/game.c
HEADERS = include/game.h

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: all
	$(RUN)

clean:
	$(RM) $(TARGET)
