#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int err() {
    printf("errno %d\n%s\n", errno, strerror(errno));
    exit(1);
}

int randyLit() {
    int randNum;
    int* pointer = &randNum;
    int recieve = open("/dev/random", O_RDONLY);

    if (recieve == -1)
        err();

    read(recieve, pointer, 4);

    return *pointer;
}

int duel(struct player_values mine, struct player_values opponent) {
    int sum = mine.troops + opponent.troops;
    int random = randyLit()%sum + 1;
    if (random > mine.troops) {
        opponent.troops += 1;
        mine.troops -= 1;
        int troops_lost = randyLit()%(mine.troops/2) + (mine.troops/10);
    }
    else {
        opponent.troops -= 1;
        mine.troops += 1;
        int troops_lost = randyLit()%(opponent.troops/2) + (opponent.troops/10);
    }
}

int wager(struct player_values mine, struct player_values opponent, int amount) {
    int random = randyLit()%2 + 1;
    if (random > mine.troops) {
        opponent.gold += amount;
        mine.gold -= amount;
    }
    else {
        opponent.gold -= amount;
        mine.gold += amount;
    }
}

int commonEvent(int random){
    if (random <= 10) {
        
    }
    else if (random <= 20) {

    }
    else if (random <= 30) {

    }
    else if (random<= 40) {

    }
    else {

    }
}

int individualLevel() {
    int random = randyLit()%100 +1;
    if (random <= 50) {
        commonEvent(random);
    }
    else if (random <= 80) {
        uncommonEvent();
    }
    else if (random <= 100) {
        rareEvent();
    }
}

int tribeLevel() {

}

int serverLevel() {

}

int getEvent() {

}