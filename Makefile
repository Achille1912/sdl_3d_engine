C = cc
CFLAGS = -Wall -Wextra -std=c11 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lm

TARGET = window
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)


run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
