CC = g++
CFLAGS = -I src/include -I src/include/SDL2 -I Header
LDFLAGS = -L src/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

OBJ = main.o Game/Player.o Menu.o

all: game

game: $(OBJ)
	$(CC) -o game $(OBJ) $(LDFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

Menu.o: Game/Menu.cpp
	$(CC) $(CFLAGS) -c Game/Menu.cpp -o Menu.o


Game/Player.o: Game/Player.cpp
	$(CC) $(CFLAGS) -c Game/Player.cpp -o Game/Player.o

clean:
	rm -f game $(OBJ)
