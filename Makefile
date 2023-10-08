run: build-2d run-2d

# Produce asm
asm:
	gcc main.c -o main.asm -O1 -S -I include -L lib -l SDL2-2.0.0


build: build-2d build-1d

build-2d:
	gcc main-2d.c -Wall -o main-2d -O1 -I include -L lib -l SDL2-2.0.0

build-1d:
	gcc main-1d.c -Wall -o main-1d -O1 -I include -L lib -l SDL2-2.0.0

run-1d:
	./main-1d

run-2d:
	./main-2d


