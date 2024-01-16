#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "challenges.h"

int main(){
    

    char *hostname = "127.0.0.1";
    char* port = "9998";
    struct addrinfo hints, *server_info;

    // Initialize hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &server_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    int client_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (client_socket == -1) {
        perror("socket");
        exit(1);
    }

    if (connect(client_socket, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        perror("connect");
        close(client_socket);
        exit(1);
    }

    char buff[1025];
    char serverBuff[1025];
    buff[0] = '\0'; // Initialize the first element to null character
    printf("Connected to server. Type 'start game' to begin.\n");

    while (1) {
        // memset(buff, 0, sizeof(buff)); // No need to use memset here

        fgets(buff, sizeof(buff) - 1, stdin);
        buff[strlen(buff) - 1] = '\0'; // Remove newline character

       if (write(client_socket, buff, strlen(buff)) == -1) {
            perror("write");
            close(client_socket);
            exit(1);
        }

        if (strcmp(buff, "start game") == 0) {
            break; // Exit the loop after sending "start game"
        }

        if(read(client_socket, serverBuff, 50) != -1){
            if(!strcmp(serverBuff, "1")){
                break;
            }
        }
    }

    close(client_socket);
    printf("Game starting...\n");
    int tribe = 0;
    int turnBegun = 0;
    int roundCount;
    printf("Time to choose your tribe! Type 0 to choose fire, and get a x1.1 multiplier to troop gain, type 1 to choose water and get a 1.1x multiplier to gold gain, and type 2 to choose grass, and get a 1.1x to population gain\n");
    while(scanf("%d", &tribe) != 1){
        printf("please enter a valid input(0, 1, or 2)");
    }
    
    struct player_values yourPlayer = {.gold = 100, .troops = 100, .cities = 2, .population = 400, .troopMulti = 1.0, .goldMulti = 1.0, .populationMulti = 1.0,};
    tribeEntry(tribe, yourPlayer);
    while(!isAlive(yourPlayer)){
        printf("\n\nRound: %d\n");
        if(!turnBegun){
            gainFromCities(yourPlayer);
            currentStatus(yourPlayer);
            turnBegun = 1;

        }
        
        while( optionsToTakeEachRound(yourPlayer) != 4){}
        turnBegun = 0;
    }


    //each turn check if alive
    //each turn run gain from cities, current status

    return 0;
}




