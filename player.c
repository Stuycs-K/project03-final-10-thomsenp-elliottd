#include <stdio.h>
#include <string.h>

int main(){
    char currentInput[100];
    int turnNum = 0;
    while(1){
        printf("Turn:%d    Type 's' to see current stats, 'r' to see and recruit troops, 'c' to see and take on challenges and duels, and 'b' to see and build buildings\n", turnNum);
        scanf("%99s", currentInput); // Reads a string from stdin until a space is encountered
        
        

        if(strcmp(currentInput, "s") == 0){
            printf("Gold = PLACEHOLDER\nPopulation = PLACEHOLDER\nGold per Turn = PLACEHOLDER\nPopulation per Turn = PLACEHOLDER\nFaction = PLACEHOLDER\nCurrent Talents = PLACEHOLDER\nGlobal Gold Ranking = PLACEHOLDER LIST\nGlobal Military Ranking = PLACEHOLDER LIST\n");
            turnNum++;
        }

        else if(strcmp(currentInput, "r") == 0){
            printf("Current Troops = PLACEHOLDER LIST WITH NUMS AND STATS\nTroops Avaliable\n type '0 numOfTroops'(Ex: 10) to select TROOP 1: PRICE\nSTATS = PLACEHOLDER LIST\nREPEAT AS NEEDED\n");
            printf("Desired troop and number('x' to cancel)");
            scanf("%99s", currentInput);
            //will not work as is, based on spaces
            printf("troopTest%s", currentInput);
            //scan to get int, run through list
            turnNum++;
            
        }

        else if(strcmp(currentInput, "c") == 0){
            printf("Turn Challenge = PLACEHOLDER\nOngoging Server Challenge = PLACEHOLDER\nPlayer List = PLACEHOLDER LIST\nFINAL BATTLE REQUIREMENTS = PLACEHOLDER\n");
            printf("Desired menu('x' to cancel)");
            scanf("%99s", currentInput);
            printf("challengeTest%s", currentInput);
            turnNum++;
            
        }

        else if(strcmp(currentInput, "b") == 0){
            printf("Current Buildings = PLACEHOLDER LIST\nBuildings avaliable = LIST WITH PRICES AND TIME\n");
            printf("Desired building('x' to cancel)");
            scanf("%99s", currentInput);
            printf("buildingTest%s", currentInput);
            turnNum++;
            
        }

        else{
            printf("Invalid input, please type 's' to see current stats, 'r' to see and recruit troops, 'c' to see and take on challenges and duels, and 'b' to see and build buildings\n");
        }
    }
    
}



