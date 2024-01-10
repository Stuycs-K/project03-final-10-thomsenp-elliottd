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

int commonEvent(int random, struct player_values input){
    if (random <= 10) {
        printf("Your kingdom faces harsh weather, you lose 20 gold.\n");
        input.gold -= 20;
    }
    else if (random <= 20) {
        printf("Bandits bombard your kingdom, you lose 10 troops.\n");
        input.troops -= 10;
    }
    else if (random <= 30) {
        printf("You receive refugees, your population grows by 20 people.\n");
        input.population += 20;
    }
    else if (random<= 40) {
        printf("Some residents hate your recent actions, your population decreases by 10 people.\n");
        input.population -= 10;
    }
    else {
        printf("Your kingdom becomes enveloped in fog, when it clears you find that nothing has happened.\n");
    }
}

int uncommonEvent(int random, struct player_values input){
    if (random <= 60) {
        printf("Your city floods and much is lost, you lose 100 gold and 50 people.\n");
        input.population -= 50;
        input.gold -= 100;
    }
    else if (random <= 70) {

    }
    else {

    }
}

int individualLevel(struct player_values input) {
    int random = randyLit()%100 +1;
    if (random <= 50) {
        commonEvent(random, input);
    }
    else if (random <= 80) {
        uncommonEvent(random, input);
    }
    else if (random <= 99) {
        rareEvent(random, input);
    }
    else {
        printf("EVERYTHING IS ON FIRE, AAAAAAAAAAH.\n");
        sleep(2);
        printf("Your kingdom is burnt to ash, you could not do anything. L.\n");
        input.population = 0;
        //ADD A FUNCTION THAT CONSTANTLY CHECKS THE VALUE OF POPULATION TO DETERMINE IF A KINGDOM IS ALIVE OR NOT
    }
}

int tribeLevel() {

}

int serverLevel() {

}

int getEvent() {

}