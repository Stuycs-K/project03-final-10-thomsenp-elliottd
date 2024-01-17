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
#include "player.h"
#include "challenges.h"
#include "server.h"

void copyPlayerData(struct player_data *dest, const struct player_data *src) {
    if (dest == NULL || src == NULL) {
        // Handle error: either dest or src is NULL
        return;
    }

    dest->gold = src->gold;
    dest->troops = src->troops;
    dest->cities = src->cities;
    dest->population = src->population;
    dest->tribe = src->tribe;
    dest->duelRequestMade = src->duelRequestMade;
    dest->selfIdentifier = src->selfIdentifier;
    dest->enemyIdentifier = src->enemyIdentifier;
    dest->succeededInFinalBattle = src->succeededInFinalBattle;
}


void copyDataToPlayerValues(struct player_values *dest, const struct player_data *src) {
    dest->gold = src->gold;
    dest->troops = src->troops;
    dest->cities = src->cities;
    dest->population = src->population;
    dest->tribe = src->tribe;
    // dest->troopMulti, dest->goldMulti, and dest->populationMulti remain unchanged
}

int receiveStringFromServer(int client_socket, char* buffer, int bufferSize) {
    int length;
    if (read(client_socket, &length, sizeof(length)) == -1) {
        perror("read length");
        return -1;
    }

    if (length > bufferSize) {
        fprintf(stderr, "Received string length (%d) exceeds buffer size (%d)\n", length, bufferSize);
        return -1;
    }

    if (read(client_socket, buffer, length) == -1) {
        perror("read string");
        return -1;
    }
    buffer[length] = '\0'; // Ensure null-termination
    printf("%s\n", buffer);
    return 0;
}
int receiveIntFromServer(int client_socket, int* value) {
    int bytesRead = read(client_socket, value, sizeof(int));
    if (bytesRead == -1) {
        perror("read");
        return -1; // Indicate error
    } else if (bytesRead == 0) {
        printf("Server closed the connection.\n");
        return -2; // Indicate server disconnection
    }
    return 0; // Success
}

int sendIntToServer(int client_socket, int value) {
    if (write(client_socket, &value, sizeof(value)) == -1) {
        perror("write");
        return -1; // Indicate error
    }
    return 0; // Success
}

int receivePlayerInit(int client_socket, struct player_init *initData) {
    if (receiveIntFromServer(client_socket, &initData->playerCount) == -1 ||
        receiveIntFromServer(client_socket, &initData->difficulty) == -1) {
        return -1; // Error occurred
    }
    return 0; // Success
}

int receivePlayerData(int client_socket, struct player_data *player) {
    int *fields[] = {
        player->gold, player->troops, player->cities, player->population,
        player->tribe, player->duelRequestMade, player->selfIdentifier,
        player->enemyIdentifier, player->succeededInFinalBattle
    };

    for (int i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        if (receiveIntFromServer(client_socket, fields[i]) == -1) {
            return -1; // Error occurred
        }
    }
    return 0; // Success
}

int sendPlayerData(int client_socket, struct player_data player) {
    int fields[] = {
        player.gold, player.troops, player.cities, player.population, 
        player.tribe, player.duelRequestMade, player.selfIdentifier,
        player.enemyIdentifier, player.succeededInFinalBattle
    };

    for (int i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        if (sendIntToServer(client_socket, fields[i]) == -1) {
            return -1; // Error occurred
        }
    }
    return 0; // Success
}

struct player_data createPlayerData(struct player_values input, int duel, int identifierSelf, int enemyIdentifier, int finalBattle) {
    struct player_data newData;

    // Copying existing fields
    newData.gold = input.gold;
    newData.troops = input.troops;
    newData.cities = input.cities;
    newData.population = input.population;
    newData.tribe = input.tribe;

    // Setting new fields
    newData.duelRequestMade = duel;
    newData.selfIdentifier = identifierSelf;
    newData.enemyIdentifier = enemyIdentifier;
    newData.succeededInFinalBattle = finalBattle;

    return newData;
}

int receivePlayerDataList(int client_socket, struct player_data** dataList, int* listSize) {
    // First, receive the number of structs
    if (receiveIntFromServer(client_socket, listSize) == -1) {
        return -1; // Error occurred
    }

    // Allocate memory for the list of player_data structs
    *dataList = malloc(*listSize * sizeof(struct player_data));
    if (*dataList == NULL) {
        perror("malloc");
        return -1; // Memory allocation error
    }

    // Then, receive each struct
    for (int i = 0; i < *listSize; i++) {
        struct player_data* player = &((*dataList)[i]);
        int* fields[] = {&player->gold, &player->troops, &player->cities, &player->population, &player->tribe,
                         &player->duelRequestMade, &player->selfIdentifier, &player->enemyIdentifier, 
                         &player->succeededInFinalBattle};

        for (int j = 0; j < sizeof(fields) / sizeof(fields[0]); j++) {
            if (receiveIntFromServer(client_socket, fields[j]) == -1) {
                free(*dataList); // Free allocated memory in case of error
                return -1; // Error occurred
            }
        }
    }
    return 0; // Success
}
int main(int argc, char *argv[]) {
    char *hostname;
    char *port = "9998";
    struct addrinfo hints, *server_info;

    // Use provided IP address or default to localhost
    if (argc > 1) {
        hostname = argv[1];
    } else {
        hostname = "127.0.0.1";
    }

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
    char testBuff[1025];
    char serverBuff[1025];
    buff[0] = '\0'; // Initialize the first element to null character
    printf("Connected to server. Waiting for game to start\n");

    fd_set read_fds;
int max_fd = client_socket > STDIN_FILENO ? client_socket : STDIN_FILENO;

while (1) {
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(client_socket, &read_fds);

    // Wait for input from either stdin or the client socket
    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
        perror("select");
        exit(EXIT_FAILURE);
    }

    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
        // Handle input from stdin
        fgets(buff, sizeof(buff) - 1, stdin);
        buff[strlen(buff) - 1] = '\0'; // Remove newline character

        char *find = strstr(buff, "start");
        if (!find) {
            find = strstr(buff, "QU");
        }

        if (find != NULL) {
            printf("%s\n", buff);
            if (write(client_socket, buff, strlen(buff)) == -1) {
                perror("write");
                close(client_socket);
                exit(1);
            }

            if (strcmp(buff, "start game") == 0) {
                break; // Exit the loop after sending "start game"
            }
        }
    }

    if (FD_ISSET(client_socket, &read_fds)) {
        // Handle input from the client socket
        int bytesRead = read(client_socket, serverBuff, 2);
        if (bytesRead == -1) {
            perror("read");
            close(client_socket);
            exit(1);
        } else if (bytesRead == 0) {
            printf("Server closed the connection.\n");
            close(client_socket);
            exit(0);
        }

        serverBuff[bytesRead - 1] = '\0'; // Null-terminate the string

        if (strstr(serverBuff, "1") != NULL) {
            break; // Exit if "1" is in the server response
        } else {
            printf("Server response: %s\n", serverBuff);
        }
    }
}
    
        // else{
        //     strcpy(buff, "no working");
        //     printf("failed\n");
            
        // }
    
    struct player_init gameInitData;
    int identifier;
    
    if (receivePlayerInit(client_socket, &gameInitData) == -1) {
        perror("setup");
    }
    if(receiveIntFromServer(client_socket, &identifier) == -1){
        perror("setup");
    }
    
    printf("Game starting...\n");
    char test[10];
    char* finder;
    
    int tribe = 0;
    int turnBegun = 0;
    int roundCount = 0;
    
   

    
    printf("%d, %d, %d\n", gameInitData.playerCount, gameInitData.difficulty, identifier);
    printf("Time to choose your tribe! Type 0 to choose fire, and get a x1.1 multiplier to troop gain, type 1 to choose water and get a 1.1x multiplier to gold gain, and type 2 to choose grass, and get a 1.1x to population gain\n");
    while(scanf("%d", &tribe) != 1){
        printf("please enter a valid input(0, 1, or 2)");
    }
    

     //read from server
    struct player_values yourPlayer = {.gold = 100, .troops = 100, .cities = 2, .population = 400, .troopMulti = 1.0, .goldMulti = 1.0, .populationMulti = 1.0,};
    struct player_data playerTransmit = createPlayerData(yourPlayer, 0, identifier, NULL, 0);
    int wantDuel = 0;
    int duellingTarget;
    int troopsLastTurn = 0;
    int goldLastTurn = 0;
    int populationLastTurn = 0;
    int troopDifference;
    int wonFinal = 0;
    
    
    tribeEntry(tribe, &yourPlayer);
    while(!isAlive(yourPlayer)){
        printf("\n\nRound: %d\n", roundCount++);
        //event
        //server event
        if(!turnBegun){
            //get player list, saved in all players
            copyDataToPlayerValues(&yourPlayer, &playerTransmit);
            gainFromCities(&yourPlayer);
            currentStatus(yourPlayer);
            turnBegun = 1;

        }
        int option = 0;
        while (1) {
            option = optionsToTakeEachRound(&yourPlayer);
            if (option == 4) {
                break;
            }

            if (option == 7) {
                wantDuel = 1;
                scanf("%d", &duellingTarget);
                printf("\nDuel initiated with %d\n", duellingTarget);
                fflush(stdout);
            }
            if(option == 5){
                wonFinal = 1;

            }
        }
        playerTransmit = createPlayerData(yourPlayer, wantDuel, identifier, duellingTarget, wonFinal);
        troopsLastTurn = playerTransmit.troops;
        goldLastTurn = playerTransmit.gold;
        populationLastTurn = playerTransmit.population;

        sendPlayerData(client_socket,playerTransmit);
        
        fflush(stdout);
        turnBegun = 0;
        struct player_data* playerList = NULL;
        int listSize = gameInitData.playerCount;
        printf("\nwaiting for other players...\n");
    // Call the function to receive the list of player data
        int result = receivePlayerDataList(client_socket, &playerList, &listSize);
    if (result != 0) {
        fprintf(stderr, "Error receiving player data list\n");
        // Handle the error appropriately
        close(client_socket);
        return -1;
    }
    
    fflush(stdout);
    copyPlayerData(&playerTransmit, & playerList[identifier]);
    if(wantDuel){
               
                fflush(stdout);
                wantDuel = 0;
                printf("%d,%d\n",playerList[identifier].troops,  troopsLastTurn);
                troopDifference = playerList[identifier].troops - troopsLastTurn;
                if(troopDifference < 0){
                    printf("You lost a duel and lost %d troops", -1 * troopDifference);
                }
                else{
                    printf("You won a duel");
                }

            }
    
    char bufferEvent[512];
    int bufferSize = sizeof(bufferEvent); // Use sizeof to ensure buffer size is correct
    printf("\n\nNEW TURN\n");
    int num = receiveStringFromServer(client_socket, bufferEvent, bufferSize);
    // Process the received data
    for (int i = 0; i < listSize; i++) {
        if(playerList[i].selfIdentifier == identifier){

        }
        else{
        printf("\nCurrent Player List: \nPlayer %d: Gold: %d, Troops: %d, Cities: %d", 
               i, playerList[i].gold, playerList[i].troops, playerList[i].cities);
               fflush(stdout);
        }
        // Add more processing as needed
    }

        //send player data to server
    }


    //each turn check if alive
    //each turn run gain from cities, current status

    return 0;
}




