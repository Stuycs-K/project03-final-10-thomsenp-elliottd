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

int tribeEntry(int tribe, struct player_values input) {
    input.troopMulti = 1.0;
    input.goldMulti = 1.0;
    input.populationMulti = 1.0;
    if (tribe == 0) {
        input.troopMulti = 1.1;
    }
    else if (tribe == 1) {
        input.goldMulti = 1.1;
    }
    else if (tribe == 2){
        input.populationMulti = 1.1;
    }
}

int commonEvent(int random, struct player_values input){
    if (random <= 13) {
        printf("Your kingdom faces harsh weather, you lose 20 gold.\n");
        input.gold -= 20;
    }
    else if (random <= 2*13) {
        printf("Bandits bombard your kingdom, you lose 10 troops.\n");
        input.troops -= 10;
    }
    else if (random <= 3*13) {
        printf("You receive refugees, your population grows by 20 people.\n");
        input.population += (int) (20*input.populationMulti);
    }
    else if (random<= 4*13) {
        printf("Some residents hate your recent actions, your population decreases by 10 people.\n");
        input.population -= 10;
    }
    else {
        printf("Your kingdom becomes enveloped in fog, when it clears you find that nothing has happened.\n");
    }
}

int uncommonEvent(int random, struct player_values input){
    if (random <= 65+8) {
        printf("Your city floods and much is lost, you lose 100 gold and 50 people.\n");
        input.population -= 50;
        input.gold -= 100;
    }
    else if (random <= 65+8*2) {
        printf("A great hero visits your kingdom, many flock to visit them. You gain 300 gold.\n");
        input.gold += 300;
    }
    else {
        printf("A cult forms in your kingdom, many kill themselves in hopes of meeting the great flying spaghetti monster. You lose 120 people.");
        input.population -= 120;
    }
}

int rareEvent(int random, struct player_values input) {
    if (random <= 94) {
        printf("A dragon attacks your kingdom! You lose 2 cities, 400 people, 320 gold, and 330 troops.\n");
        input.cities -= 2;
        input.population -= 100;
        input.gold -= 320;
        input.troops -= 330;
    }
    else {
        printf("Mr.K has smiled upon you. You gain 400 gold and two cities.\n");
        input.cities += 2;
        input.gold += 400;
        input.population += 400;
    }
}

int tribeLevel(struct player_values input) {
    int random = randyLit()%100 +1;
    if (random <= 65) {read
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

int serverLevel() {
    int random = randyLit()%3 + 1;
    int final;
    if (random <= 1) {
        printf("The goblin empire is aiming to conquer everything. Your nation is in its warpath.\n");
        printf("What will you do?\n");
        printf("Option 1: The goblins are incompetent, do nothing.\n");
        printf("Option 2: Shelter in and prepare all of your cities for a siege.\n");
        printf("Option 3: WAAAAAAR!!!!!!! (Take the battle to them with a preemptive strike).\n");
        printf("Option 4: Attempt diplomacy with the goblins (Do you have a good translator?).\n");
        printf("What path will your kingdom take, please select an option: ");
        scanf("%d", &final);
        return final;
    }
    else if (random <= 2) {
        printf("A plague is sweeping the continent. Your people live in terrible fear of the Miasma of Doom\n");
        printf("What will you do?\n");
        printf("Option 1: Miasma is nothing, why should I take action?\n");
        printf("Option 2: Spend half of your nation's gold to gather the greatest physicians in the land.\n");
        printf("Option 3: Execute all of the sick and burn their bodies.\n");
        printf("Option 4: Turn to the church and believe in the power of god(s).\n");
        printf("What path will your kingdom take, please select an option: ");
        scanf("%d", &final);
        return final;
    }
    else {
        printf("\n");
        printf("What will you do?\n");
        printf("Option 1: \n");
        printf("Option 2: \n");
        printf("Option 3: \n");
        printf("Option 4: \n");
        printf("What path will your kingdom take, please select an option: ");
        scanf("%d", &final);
        return final;
    }
}

int getEvent() {

}

int isAlive(struct player_values input) { //Returns true if the character is still alive
    if (input.population == 0) {
        return 1;
    }
    return 0;
}

