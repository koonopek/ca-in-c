build-and-run: build run

# Produce asm
asm:
	gcc main.c -o main.asm -O1 -S -I include -L lib -l SDL2-2.0.0

build:
	gcc main.c -Wall -o main -O1 -I include -L lib -l SDL2-2.0.0

run:
	./main


