#ifndef CHALLENGES_H
#define CHALLENGES_H
struct player_values {
    int gold;
    int troops;
    int cities;
    int population;
    int tribe;
    double troopMulti; //option 0 WHICH IS FIRE BECAUSE OF THE HEAT OF BATTLE
    double goldMulti; //option 1 WHICH IS GOLD BECAUSE OF LIQUIDITY
    double populationMulti; //option 2 WHICH IS GRASS BECAUSE OF STRONG ROOTS
};
int err();
int randyLit();
int duel(struct player_values mine, struct player_values opponent);
int wager(struct player_values mine, struct player_values opponent, int amount);
int tribeEntry(int tribe, struct player_values input);
int tribeLevel(struct player_values input);
int serverLevel(struct player_values input);
int isAlive(struct player_values input);
int gainFromCities(struct player_values input);
int currentStatus(struct player_values input);
int buyTroops(int numTroops, struct player_values input);
int buyCities(int numCities, struct player_values input);
int optionsToTakeEachRound(struct player_values input);
#endif
