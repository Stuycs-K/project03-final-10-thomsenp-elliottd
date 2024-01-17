#ifndef CHALLENGES_H
#define CHALLENGES_H

#include "player.h" // Include if necessary for player_data structure

// Structure definition
struct player_values {
    int gold;
    int troops;
    int cities;
    int population;
    int tribe;
    double troopMulti;
    double goldMulti;
    double populationMulti;
};

// Function prototypes
int err();
int randyLit();
int* duel(struct player_data* mine, struct player_data* opponent);
int wager(struct player_values* mine, int amount);
int tribeEntry(int tribe, struct player_values* input);
void commonEvent(int random, struct player_values* input, char* message, int difficulty);
void uncommonEvent(int random, struct player_values* input, char* message, int difficulty);
void rareEvent(int random, struct player_values* input, char* message, int difficulty);
int tribeLevel(struct player_data* input, char* message, int difficulty);
int isAlive(struct player_values input);
int gainFromCities(struct player_values* input);
int currentStatus(struct player_values input);
int buyTroops(int numTroops, struct player_values* input);
int buyCities(int numCities, struct player_values* input);
int optionsToTakeEachRound(struct player_values* input);

#endif // CHALLENGES_H
