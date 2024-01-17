#ifndef SERVER_H
#define SERVER_H

#include "player.h"  // Assuming player.h contains relevant structures and prototypes

// Declaration of the player_init struct
struct player_init {
    int playerCount;
    int difficulty;
};

// Function Prototypes
struct player_values convertDataToValues(const struct player_data* data);
int sendPlayerInit(int client_socket, struct player_init initData);
int sendIntToClient(int client_socket, int value);
int receiveIntFromClient(int client_socket, int* value);
int sendPlayerData(int client_socket, struct player_data player);
int receivePlayerData(int client_socket, struct player_data *player);
void removeElement(int arr[], int *size, int elementToRemove);
int sendPlayerDataList(int client_socket, struct player_data* dataList, int listSize);
int sendStringToClient(int client_socket, const char* str);

#endif // SERVER_H
