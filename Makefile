CC=gcc-13
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=./BrickBreaker

.PHONY: all
all: $(EXECUTABLE)
	$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) -F/Library/Frameworks -framework SDL2 $^

$(OBJECTS): %.o: %.c
	$(CC) -c -o $@ -Wall -Wextra -std=c99 -F/Library/Frameworks -pedantic $<

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)