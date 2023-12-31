# A simple Makefile for compiling small SDL projects

theGame: main.o game_engine.o LoadMenu.o Network.o obstacle.o player.o world.o client_game_engine.o LoadMedia.o input.o lobby.o Store.o
	gcc -o theGame main.o game_engine.o LoadMenu.o Network.o obstacle.o player.o world.o client_game_engine.o LoadMedia.o input.o lobby.o Store.o -lSDL2 -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2_mixer
main.o: main.c
	gcc -c main.c
game_engine.o: game_engine.c
	gcc -c game_engine.c
LoadMenu.o: LoadMenu.c
	gcc -c LoadMenu.c
Network.o: Network.c
	gcc -c Network.c
obstacle.o: obstacle.c
	gcc -c obstacle.c
player.o: player.c
	gcc -c player.c
world.o: world.c
	gcc -c world.c
client_game_engine.o: client_game_engine.c
	gcc -c client_game_engine.c
LoadMedia.o: LoadMedia.c
	gcc -c LoadMedia.c
input.o: input.c
	gcc -c input.c
lobby.o: lobby.c
	gcc -c lobby.c
Store.o: Store.c
	gcc -c Store.c