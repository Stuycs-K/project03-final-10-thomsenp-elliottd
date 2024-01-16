.PHONY: compile client server compClient compServer clean clone
IP ?= 127.0.0.1
compile: compPlayer compServer
player: compPlayer
	@./player $(IP)
server: compServer
	@./server

compPlayer: client.o networking.o
	@gcc -o client client.o prompt.o data.o challenges.o -w
compServer: server.o networking.o
	@gcc -o server server.o prompt.o data.o challenges.o -w

player.o: client.c networking.h
	@gcc -c client.c
server.o: server.c networking.h
	@gcc -c server.c
prompt.o: prompt.c prompt.h
	@gcc -c prompt.c
data.o: data.c data.h
	@gcc -c data.c
challenges.o: challenges.c challenges.h
	@gcc -c challenges.c


clean:
	@rm -f server client
	@rm -f *.o
	@rm -f *~
	@rm -f [0-9]*
