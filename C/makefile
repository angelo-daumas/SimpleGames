CC=gcc

CFLAGS=-Wall -pedantic -D_GNU_SOURCE=1 -D_REENTRANT
SDL_FLAGS=-lSDL2
IMAGE_FLAGS=-lSDL2_image
GRAPHIC_FLAGS=-lSDL_gfx
MIXER_FLAGS=-lSDL2_mixer
TEXT_FLAGS=-lSDL2_ttf
MATH_FLAGS=-lm
#LFLAGS=-L/usr/lib
#IFLAGS=-I/usr/include/SDL
SOURCES=main.c g_func.c globals.c game_states.c file_IO.c
HEADERS=g_func.h defs.h globals.h game_states.h file_IO.h

main: $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) $(CFLAGS) $(MATH_FLAGS) $(SDL_FLAGS) $(IMAGE_FLAGS) $(MIXER_FLAGS) $(TEXT_FLAGS) -o main
	make main

optimized: $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) $(CFLAGS) $(MATH_FLAGS) $(SDL_FLAGS) $(IMAGE_FLAGS) $(MIXER_FLAGS) $(TEXT_FLAGS) -O3 -o optimized

clean:
	rm -rf *.o *.exe main
	rm -rf *.o *.exe optimized
