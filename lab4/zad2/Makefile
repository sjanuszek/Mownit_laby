CC = gcc
CFLAGS = -O3 -Wall
LIBS = -lm

TARGET = ISING 

all: $(TARGET)

$(TARGET): ISING.c
	$(CC) $(CFLAGS) -o $(TARGET) ISING.c $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean test
