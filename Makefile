CC=gcc
OUT=pendulum.bin

SRC=main.c

all:
	$(CC) $(SRC) -o $(OUT) -lSDL2 -lGL -lm

clean:
	rm $(OUT)
