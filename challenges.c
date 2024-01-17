#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "player.h"
#include "challenges.h"


int err() {
    printf("errno %d\n%s\n", errno, strerror(errno));
    exit(1);
}


int randyLit() {
    unsigned int randNum;
    int recieve = open("/dev/urandom", O_RDONLY);

    if (recieve == -1)
        err();

    read(recieve, &randNum, sizeof(randNum));
    close(recieve);

    return abs((int)randNum);  // Make sure the number is always positive
}

int* duel(struct player_data* mine, struct player_data* opponent) {
    static int result[2];
    int sum = mine->troops + opponent->troops;
    int random = randyLit() % sum + 1;
    int troopsLost;

    if (random > mine->troops) {
        troopsLost = randyLit() % (mine->troops / 2) + (mine->troops / 10);
        mine->troops -= troopsLost;
        result[0] = 0;  // Opponent wins
        result[1] = troopsLost;
    } else {
        troopsLost = randyLit() % (opponent->troops / 2) + (opponent->troops / 10);
        opponent->troops -= troopsLost;
        result[0] = 1;  // You win
        result[1] = troopsLost;
    }

    return result;
}

int wager(struct player_values* mine, int amount) { //fifty/fifty chance and beting gold
    int random = randyLit()%2 + 1;
    if (random == 1) {
       
        mine->gold -= amount;
        printf("You lost %d gold\n", amount);
    }
    else {
        
        mine->gold += amount;
        printf("You won %d gold\n", amount);
    }
}

int tribeEntry(int tribe, struct player_values* input) { //This command edits a player's multi value for whatever faction/tribe they chose
    input->troopMulti = 1.0;
    input->goldMulti = 1.0;
    input->populationMulti = 1.0;
    if (tribe == 0) {
        input->troopMulti = 1.1; //This is fire because they do well in the heat of battle
        input->tribe = 0;
    }
    else if (tribe == 1) {
        input->goldMulti = 1.1; //This is water because it keeps their assets liquid
        input->tribe = 1;
    }
    else if (tribe == 2){
        input->populationMulti = 1.1; //This is grass because their nation is growing strong roots
        input->tribe = 2;
    }
}

void commonEvent(int random, struct player_values* input, char* message, int difficulty) {
    if (random <= 13) {
        int goldLoss = (int)(20 * difficulty / 5.0);
        sprintf(message, "Your kingdom faces harsh weather, you lose %d gold.\n", goldLoss);
        input->gold -= goldLoss;
    }
    else if (random <= 2 * 13) {
        int troopLoss = (int)(10 * difficulty / 5.0);
        sprintf(message, "Bandits bombard your kingdom, you lose %d troops.\n", troopLoss);
        input->troops -= troopLoss;
    }
    else if (random <= 3 * 13) {
        int populationGain = (int)(20 * 1 / difficulty) + 1;
        sprintf(message, "You receive refugees, your population grows by %d people.\n", populationGain);
        input->population += populationGain;
    }
    else if (random <= 4 * 13) {
        int populationLoss = (int)(10 * difficulty / 5.0);
        sprintf(message, "Some residents hate your recent actions, your population decreases by %d people.\n", populationLoss);
        input->population -= populationLoss;
    }
    else {
        sprintf(message, "Your kingdom becomes enveloped in fog, when it clears you find that nothing has happened.\n");
    }
}
void uncommonEvent(int random, struct player_values* input, char* message, int difficulty) {
    if (random <= 65 + 8) {
        int goldLoss = (int)(100 * difficulty / 5.0);
        int populationLoss = (int)(50 * difficulty / 5.0);
        sprintf(message, "Your city floods and much is lost, you lose %d gold and %d people.\n", goldLoss, populationLoss);
        input->population -= populationLoss;
        input->gold -= goldLoss;
    }
    else if (random <= 65 + 8 * 2) {
        int goldGain = (int)(300 * input->goldMulti * difficulty / 5.0);
        sprintf(message, "A great hero visits your kingdom, many flock to visit them. You gain %d gold.\n", goldGain);
        input->gold += goldGain;
    }
    else {
        int populationLoss = (int)(120 * difficulty / 5.0);
        sprintf(message, "A cult forms in your kingdom, many kill themselves in hopes of meeting the great flying spaghetti monster. You lose %d people.\n", populationLoss);
        input->population -= populationLoss;
    }
}

void rareEvent(int random, struct player_values* input, char* message, int difficulty) {
    if (random <= 94) {
        int cityLoss = (int)(2 * difficulty / 5.0);
        int populationLoss = (int)(400 * difficulty / 5.0);
        int goldLoss = (int)(320 * difficulty / 5.0);
        int troopLoss = (int)(330 * difficulty / 5.0);
        sprintf(message, "A dragon attacks your kingdom! You lose %d cities, %d people, %d gold, and %d troops.\n", cityLoss, populationLoss, goldLoss, troopLoss);
        input->cities -= cityLoss;
        input->population -= populationLoss;
        input->gold -= goldLoss;
        input->troops -= troopLoss;
    }
    else {
        int goldGain = (int)(400 * input->goldMulti * difficulty / 5.0);
        int cityGain = 2;  // Assuming city gain is not affected by difficulty
        int populationGain = (int)(400 * input->populationMulti * difficulty / 5.0);
        sprintf(message, "Mr.K has smiled upon you. You gain %d gold and %d cities.\n", goldGain, cityGain);
        input->cities += cityGain;
        input->gold += goldGain;
        input->population += populationGain;
    }
}

int tribeLevel(struct player_data* input, char* message, int difficulty) { //Determines whether the event for that round should be common
    int random = randyLit()%100 +1;
    if (random <= 65) { //There is a 65% chance of a common event when there is an individual event
      commonEvent(random, input, message, difficulty);
    }
    else if (random <= 89) { //There is a 24% chance of an uncommon event when there is an individual event
        uncommonEvent(random, input, message, difficulty);
    }
    else if (random <= 99) { //There is a 10% chance of a rare event when there is an individual event
        rareEvent(random, input, message, difficulty);
    }
    else { //There is a 1% chance of immediately losing when there is an individual event.
        sprintf(message, "EVERYTHING IS ON FIRE, AAAAAAAAAAH.\n Your kingdom is burnt to ash, you could not do anything. L.\n");
        input->population = 0;
        //ADD A FUNCTION THAT CONSTANTLY CHECKS THE VALUE OF POPULATION TO DETERMINE IF A KINGDOM IS ALIVE OR NOT
    }
}
//REMOVED AS WAS TOO UNWIEDLY FOR USER COMPRHENSION, TOOK 3 TURNS TO PROCESS
// int eventInput(int random, int response, struct player_values input) {
//   if (random <= 1) { //Goblin Empire
//     if (response == 1) { //Ignore
//       printf("The goblins are mighty and powerful!!!! But they also never even heard of your kingdom so they pass you by.\n");
//       //no changes to the kingdom since nothing happened
//     }
//     else if (response == 2) { //Prepare for siege
//       printf("The goblins hear word of your preparations and launch a massive assault.\n");
//       printf("Your kingdom is able to resist the goblins until they need to retreat because of other forces.\n");
//       printf("You have lost 350 gold.");
//       input.gold -= 350;
//     }
//     else if (response == 3) { //Preemptive strike
//       printf("The massive goblin army is caught off guard by your offensive measures!\n");
//       printf("Your army fights admirably and wins you the battle but many troops are lost.\n");
//       printf("You gain 600 gold but sacrifice 400 troops\n");
//       input.gold += (int) (600 * input.goldMulti);
//       input.troops -= 400;
//     }
//     else { //Diplomacy
//       printf("Your translator was lying about speaking the goblin language and has greatly offended their emperor.\n");
//       printf("Your nation now must face the full force of the goblin army and you suffer many losses.\n");
//       printf("You lose 400 citizens, 500 troops, and 650 gold.\n");
//       input.gold -= 650;
//       input.population -= 400;
//       input.troops -= 500;
//     }
//   }
//   else if (random <= 2) { //Plague
//     if (response == 1) { //Ignore
//       printf("The plague ravages your nation and you lose countless people.\n");
//       printf("You lose 700 citizens.\n");
//       input.population -= 700;
//     }
//     else if (response == 2) { //Buy doctors
//       printf("You spend half of your treasury to get the best doctors from accross the world and stem the spread of the virus\n");
//       printf("You have lost %d gold.", input.gold/2);
//       input.gold /= 2;
//     }
//     else if (response == 3) { //Kill the sick
//       printf("Countless citizens die but you successfully stop the disease.\n");
//       printf("You lose 800 from your population and 200 troops\n");
//       input.troops -= 200;
//       input.population -= 800;
//     }
//     else { //PRAY
//       printf("The church does not do very much in terms of giving you \"tangible results\"\n");
//       printf("You lose 550 gold and 600 people\n");
//       input.gold -= 550;
//       input.population -= 600;
//     }
//   }
//   else { //SHARKNADO!!!
//     if (response == 1) { //Ignore
//       printf("The Sharknado decimates your kingdom.\n");
//       printf("You lose 600 citizens and two cities\n");
//       input.population -= 600;
//       input.cities -= 2;
//     }
//     else if (response == 2) { //Send in the troops
//       printf("What did you expect the troops to do about a tornado of sharks?\n");
//       printf("The sharknado decimates your troops and continues on to your kingdom.");
//       printf("You have lost 400 troops, 400 people, and a city.");
//       input.troops -= 400;
//       input.population -= 400;
//       input.cities -= 1;
//     }
//     else if (response == 3) { //Send in the treasury
//       printf("The sharks die as they're thrown into the gold and the tornado is diverted by the power of money.\n");
//       printf("You lose 500 gold\n");
//       input.gold -= 500;
//     }
//     else { //Send in the people
//       printf("The sharks just... they just eat a lot of your people.\n");
//       printf("You lose 700 people\n");
//       input.population -= 700;
//     }
//   }
// }

// int serverLevel(struct player_values input) { //There are three potential serverLevel events
//     int random = randyLit()%3 + 1;            //Should definitely be restricted from happening even with a low likelihood until a certain number of rounds have passed
//     int final;
//     if (random <= 1) {
//         printf("The goblin empire is aiming to conquer everything. Your nation is in its warpath.\n");
//         printf("What will you do?\n");
//         printf("Option 1: The goblins are incompetent, do nothing.\n");
//         printf("Option 2: Shelter in and prepare all of your cities for a siege.\n");
//         printf("Option 3: WAAAAAAR!!!!!!! (Take the battle to them with a preemptive strike).\n");
//         printf("Option 4: Attempt diplomacy with the goblins (Do you have a good translator?).\n");
//         printf("What path will your kingdom take, please select an option: ");
//         scanf("%d", &final);
//         eventInput(random, final, input); //Function to determine the outcome of the user's choice
//         return final; //Returns what the player has selected
//     }
//     else if (random <= 2) {
//         printf("A plague is sweeping the continent. Your people live in terrible fear of the Miasma of Doom\n");
//         printf("What will you do?\n");
//         printf("Option 1: Miasma is nothing, why should I take action?\n");
//         printf("Option 2: Spend half of your nation's gold to gather the greatest physicians in the land.\n");
//         printf("Option 3: Execute all of the sick and burn their bodies.\n");
//         printf("Option 4: Turn to the church and believe in the power of god(s).\n");
//         printf("What path will your kingdom take, please select an option: ");
//         scanf("%d", &final);
//         eventInput(random, final, input); //Function to determine the outcome of the user's choice
//         return final; //Returns what the player has selected
//     }
//     else {
//         printf("SHARKNADO!!!!!!\n");
//         printf("What will you do?\n");
//         printf("Option 1: What's a Shark...? OH GOD!\n");
//         printf("Option 2: Send in the troops.\n");
//         printf("Option 3: Send in the treasury.\n");
//         printf("Option 4: Send in the... people I guess?\n");
//         printf("What path will your kingdom take, please select an option: ");
//         scanf("%d", &final);
//         eventInput(random, final, input); //Function to determine the outcome of the user's choice
//         return final; //Returns what the player has selected
//     }
// }



int isAlive(struct player_values input) { //Returns true if the character is still alive
    if (input.population == 0) {
        return 1;
    }
    return 0;
}

int gainFromCities(struct player_values* input) { //Used every round because the cities pay taxes to you and grow over time
  input->gold += (int) (100 * input->goldMulti * input->cities);
  input->population += (int) (50 * input->populationMulti * input->cities); //Bellow we update the player on their gains
  printf("You have gained %d gold and %d people from your cities.\n", (int) (100 * input->goldMulti * input->cities), (int) (50 * input->populationMulti * input->cities));
}

int currentStatus(struct player_values input) { //Should be called every round before we offer the options for what to do so that the player knows what is up
  printf("Here is your kingdom's current status readout:\n");
  if(input.tribe == 0){
    printf("Tribe: Fire\n");
  }
  else if(input.tribe == 1)
  {printf("Tribe: Water\n");}
  else{
    printf("Tribe: Grass\n");
  }
  printf("Gold: %d\n", input.gold);
  printf("Population: %d\n", input.population);
  printf("Troops: %d\n", input.troops);
  printf("Cities: %d\n", input.cities);
}

int buyTroops(int numTroops, struct player_values* input) { //Used as an option for an action to purchase troops
  if (input->gold >= numTroops * 10) {                      //Must sacrafice as many member of your population as you want troops
    input->gold -= numTroops * 10;                          //Troops cost 10 gold each (flat fee)
    input->troops += (int) (numTroops * input->troopMulti);
    input->population -= numTroops;
    printf("You have spent %d gold to buy %d troops and lost %d citizens.\n", numTroops * 10, (int) (numTroops * input->troopMulti), numTroops);
    return 0;
  }
  else {
    printf("You do not have enough gold to purchase that many troops.\n");
    return 1;
  }
}

int buyCities(int numCities, struct player_values* input) { //Each purchase of a city nets 200 population increase
  if (input->gold >= numCities * 150) {                     //A city costs 150 gold so that the city pays for itself twice over in three rounds
    input->gold -= numCities * 150;
    input->cities += numCities;
    input->population += (int) (200 * input->populationMulti * numCities);
    printf("You have spent %d gold to buy %d cities and gained %d citizens.\n", numCities * 150, numCities, (int) (200 * input->populationMulti * numCities));
    return 0;
  }
  else {
    printf("You do not have enough gold to purchase that many cities.\n");
    return 1;
  }
}


int optionsToTakeEachRound(struct player_values* input) { //Should likely be run after the event for that round
  printf("\n\nWhat actions would you like to take?\n");
  printf("Option 1: See status\n");
  printf("Option 2: Buy troops (10 gold and 1 citizen per troop)\n");
  printf("Option 3: Buy cities (150 gold per city)\n");
  printf("Option 4: Proceed to the next round\n");
  printf("Option 5: Challenge final boss\n");
  printf("Option 6: Wager\n");
  printf("Option 7: Duel\n");
  printf("What option do you select? ");

  int final;
  int holding;

  scanf("%d", &final);
  if (final == 1) {
    currentStatus(*input);
    final = optionsToTakeEachRound(input);
  }
  else if (final == 2) {
    printf("How many troops would you like to buy? ");
    scanf("%d", &holding);
    if (buyTroops(holding, input)) {
      final = optionsToTakeEachRound(input);
    }
  }
  else if (final == 3) {
    printf("How many cities would you like to buy? ");
    scanf("%d", &holding);
    if (buyCities(holding, input)) {
      final = optionsToTakeEachRound(input);
    }
  }
  else if (final == 4) {
    //I do not know if we should do anything here, I presume not since the player did not want to but maybe we should make some indicator for skipping
    //If nothing else it helps with the final else statement at the end of this function
  }
  else if (final == 5) {
    if (input->troops >= 5000 && input->cities >= 6 && input->population >= 6500) {
      return final;
    }
    else {
      printf("You are not yet qualified for the final boss which requires 5000 troops, 6 cities, and 6500 citizens at least to challenge\n");
      printf("Please try again after growing your kingdom.\n");
      final = optionsToTakeEachRound(input);
    }
  }
  else if (final == 6) {
     printf("How much would you like to wager(50 percent chance of losing wager amount 50 percent chance of gaining wager amount0)\n ");
    scanf("%d", &holding);
    wager(input, holding);
  }
   else if (final == 7) {
    printf("Which number on the player ranking do you want to challenge? (Please enter the number twice for confirmation)");
    
    //input
   }
  else {
    
    printf("You did not select a valid option, please try again.\n");
    final = optionsToTakeEachRound(input);
  }
  return final; //Returns the value for use again and again
}

