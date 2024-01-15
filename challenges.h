#ifndef CHALLENGES_H
#define CHALLENGES_H
int err();
int randyLit();
int duel(struct player_values player);
int wager(struct player_values mine, struct player_values opponent, int amount);
int tribeEntry(int tribe, struct player_values input);
int tribeLevel(struct player_values input)
int serverLevel(struct player_values input);
int isAlive(struct player_values input)
int gainFromCities(struct player_values input);
int buyTroops(int numTroops, struct player_values input);
int buyCities(int numCities, struct player_values input);
#endif
