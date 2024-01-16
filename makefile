.PHONY: compile client server compClient compServer clean clone
IP ?= 127.0.0.1
compile: compPlayer compServer
player: compPlayer
	@./player $(IP)
server: compServer
	@./server

compPlayer: player.o data.o challenges.o
	@gcc -o player player.o data.o challenges.o -w
compServer: server.o data.o challenges.o
	@gcc -o server server.o data.o challenges.o -w

player.o: player.c challenges.h
	@gcc -c player.c
server.o: server.c challenges.h
	@gcc -c server.c
data.o: data.c data.h
	@gcc -c data.c
challenges.o: challenges.c challenges.h player.o
	@gcc -c challenges.c 


clean:
	@rm -f server player
	@rm -f *.o
	@rm -f *~
	@rm -f [0-9]*
