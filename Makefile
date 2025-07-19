CC = cc 
CFLAGS = -Wall -Wextra

SRCS = breakout.c
TARGET = breakout

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -lm -lSDL3 -Wl,-rpath,/usr/local/lib -o $(TARGET)

.PHONY: clean run

run: $(TARGET)
	./$(TARGET)

clean: 
	rm $(TARGET)

