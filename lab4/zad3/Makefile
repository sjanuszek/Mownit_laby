CC = gcc
CFLAGS = -O3 -Wall
LIBS = -lm

TARGET = SUDOKU 

all: $(TARGET)

$(TARGET): SUDOKU.c
	$(CC) $(CFLAGS) -o $(TARGET) SUDOKU.c $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean test
