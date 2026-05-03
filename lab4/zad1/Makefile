CC = gcc
CFLAGS = -O3 -Wall
LIBS = -lm

TARGET = TSP

all: $(TARGET)

$(TARGET): TSP.c
	$(CC) $(CFLAGS) -o $(TARGET) TSP.c $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean test
