CC = gcc
CFLAGS = -Wextra -Werror -pedantic -fsanitize=address -fanalyzer
NCURSES = -lncurses
He-s-gonna-cuss = -Wall -std=c11

TARGET = game_of_life
SRC := $(shell find . -name "*.c")

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) $(NCURSES) $(CFLAGS)
	./a.out

clean:
	rm -rf ./a.out