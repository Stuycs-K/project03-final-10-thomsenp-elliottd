#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "challenges.h"
#include "server.h"
#include "player.h"
struct player_values convertDataToValues(const struct player_data* data) {
    struct player_values values;

    // Copying fields from player_data to player_values
    values.gold = data->gold;
    values.troops = data->troops;
    values.cities = data->cities;
    values.population = data->population;
    values.tribe = data->tribe;

    // Set default or calculated values for the additional fields
    values.troopMulti = 1.0;  // Example default value
    values.goldMulti = 1.0;   // Example default value
    values.populationMulti = 1.0;  // Example default value

    // Add additional logic here if these values need to be calculated differently

    return values;
}

int sendPlayerInit(int client_socket, struct player_init initData) {
    if (sendIntToClient(client_socket, initData.playerCount) == -1 ||
        sendIntToClient(client_socket, initData.difficulty) == -1){
        return -1; // Error occurred
    }
    return 0; // Success
}
int sendIntToClient(int client_socket, int value) {
    if (write(client_socket, &value, sizeof(value)) == -1) {
        perror("write");
        return -1; // Indicate error
    }
    return 0; // Success
}

int receiveIntFromClient(int client_socket, int* value) {
    int bytesRead = read(client_socket, value, sizeof(int));
    if (bytesRead == -1) {
        perror("read");
        return -1; // Indicate error
    } else if (bytesRead == 0) {
        printf("Client closed the connection.\n");
        return -2; // Indicate closed connection
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
        if (sendIntToClient(client_socket, fields[i]) == -1) {
            return -1; // Error occurred
        }
    }
    return 0; // Success
}

int receivePlayerData(int client_socket, struct player_data *player) {
    int *fields[] = {
        &player->gold, &player->troops, &player->cities, &player->population,
        &player->tribe, &player->duelRequestMade, &player->selfIdentifier,
        &player->enemyIdentifier, &player->succeededInFinalBattle
    };

    for (int i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        if (receiveIntFromClient(client_socket, fields[i]) == -1) {
            return -1; // Error occurred
        }
    }
    return 0; // Success
}
void removeElement(int arr[], int *size, int elementToRemove) {
    int i, j;
    int newSize = *size;
   int elementFound = 0;
    for (i = 0; i < newSize && !elementFound; i++) {
        if (arr[i] == elementToRemove) {
            
           elementFound = 1;
            
            for (j = i; j < newSize - 1 && arr[j + 1] != NULL; j++) {
                arr[j] = arr[j + 1];
                
            }
            arr[j+ 1] = 0;
            }
            // Decrease the size of the array
            i--; // Recheck the current index since elements have shifted
        
    }
    close(elementToRemove);
    *size = newSize; // Update the size of the array
}

int sendPlayerDataList(int client_socket, struct player_data* dataList, int listSize) {
    // First, send the number of structs
    if (sendIntToClient(client_socket, listSize) == -1) {
        return -1;
    }

    // Then, send each struct
    for (int i = 0; i < listSize; i++) {
        struct player_data player = dataList[i];
        int fields[] = {player.gold, player.troops, player.cities, player.population, player.tribe,
                        player.duelRequestMade, player.selfIdentifier, player.enemyIdentifier, 
                        player.succeededInFinalBattle};

        for (int j = 0; j < sizeof(fields) / sizeof(fields[0]); j++) {
            if (sendIntToClient(client_socket, fields[j]) == -1) {
                return -1; // Error occurred
            }
        }
    }
    return 0; // Success
}

int sendStringToClient(int client_socket, const char* str) {
    int length = strlen(str) + 1; // +1 for null terminator
    if (write(client_socket, &length, sizeof(length)) == -1) {
        perror("write length");
        return -1; // Indicate error
    }

    if (write(client_socket, str, length) == -1) {
        perror("write string");
        return -1; // Indicate error
    }

    return 0; // Success
}

int main(){
    int counter = 0;
    
    char line_buff[256];
    int PLAYERCOUNT;
    int difficulty;

    printf("Please type the number of expected players and the difficulty as a number 1-10   (Ex: 5 5 is 5 players at average difficulty)\n\n");
    
    fflush(stdout);
    fgets(line_buff,255,stdin);

    sscanf(line_buff,"%d %d",&PLAYERCOUNT, &difficulty);
   
    printf("At any time after the first player joins you can start the game by typing 'start game'\n\n");
    
    
    struct addrinfo * hints, * results;
    hints = calloc(1,sizeof(struct addrinfo));
    char* PORT = "9998";


    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM; //TCP socket
    hints->ai_flags = AI_PASSIVE; //only needed on server
    getaddrinfo(NULL, PORT , hints, &results);  //NULL is localhost or 127.0.0.1

    //create socket
    int listen_socket = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    //this code allows the port to be freed after program exit.  (otherwise wait a few minutes)
    int yes = 1;
    if ( setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 ) {
        printf("sockopt  error\n");
        printf("%s\n",strerror(errno));
        exit(-1);
    }

    int err = bind(listen_socket, results->ai_addr, results->ai_addrlen);
    if(err == -1){
        printf("Error binding: %s",strerror(errno));
        exit(1);
    }
    listen(listen_socket, PLAYERCOUNT);
    printf("Listening on port %s\n",PORT);

    socklen_t sock_size;
    struct sockaddr_storage client_address;
    sock_size = sizeof(client_address);
    int currentClients[PLAYERCOUNT];
    fd_set read_fds;
    // int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    // fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    char buff[1025]="";
    int notStarting = 1;
    counter = 0;
    
    while(notStarting){
        printf("scanning for players... type start game to start the game\n");
        
       int fdMax = -1;
        FD_ZERO(&read_fds);
        FD_SET(listen_socket, &read_fds);
        if(listen_socket > fdMax){
            fdMax = listen_socket;
        }
        FD_SET(STDIN_FILENO, &read_fds);
        if(STDIN_FILENO > fdMax){
            fdMax = STDIN_FILENO;
            
        }
       
        for (int i = 0; i < counter; i++) {
            if (currentClients[i] != -1){
                FD_SET(currentClients[i], &read_fds);
                if (currentClients[i] > fdMax) {
                    fdMax = currentClients[i];
                }
            }
        }
         
       

        
       
        if(counter >= PLAYERCOUNT){
            notStarting = 0;
            
           
        }
        

        else{
             int i = select(fdMax + 1, &read_fds, NULL, NULL, NULL);
            if (i < 0) {
                perror("select");
                exit(EXIT_FAILURE);
            }

             if (FD_ISSET(listen_socket, &read_fds)) {
            
            fflush(stdout);
            //accept the connection
            int connectingClient = accept(listen_socket,(struct sockaddr *)&client_address, &sock_size);
            currentClients[counter++] = connectingClient;
            printf("Connected, to client %d\n", counter);
            fflush(stdout);
            
            
            
        }

             if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                if(fgets(buff, sizeof(buff), stdin) == NULL){
                    printf("stdin failing");
                }
                buff[strlen(buff)-1]=0;
                if(!strcmp(buff, "start game")){
                    printf("\nGAME STARTING\n");
                    fflush(stdout);
                    notStarting = 0;
                }
                else{
                    printf("%s\n", buff);
                    
                }
        }
        //if standard in, use fgets
            
        // if socket
       

         
        

       
        char clientString[256];
        for(int i = 0;i< counter;i++){
           
             if (FD_ISSET(currentClients[i], &read_fds)) {
                if(read(currentClients[i], clientString,255) != 0){
                    char *found = strstr(clientString, "start game");

                    printf("looking at clients\n");
                    fflush(stdout);

                    if(found != NULL){
                        printf("Recieved");
                        fflush(stdout);
                        notStarting = 0;
                    }
                    else if(!strcmp(clientString, "QUIT")){
                        printf("trying to delete");
                        fflush(stdout);
                        removeElement(currentClients, sizeof(currentClients) / sizeof(currentClients[0]),currentClients[i] );
                        printf("Client %d quit", ++i);
                    }
                   
             }

              else{

                        printf("trying to delete -> recieved %s", clientString);
                        fflush(stdout);
                        printf("%d\n",sizeof(currentClients)/ sizeof(currentClients[0]) );
                        fflush(stdout);
                        removeElement(currentClients, sizeof(currentClients)/ sizeof(currentClients[0]),currentClients[i] );
                        printf("Client %d quit", ++i);
                    }
             }
            

        }
        }
    }
    
    fflush(stdout);
    struct player_init gameStarter = {.playerCount = counter, .difficulty = difficulty};
    char sendStr[] = "1";
   
     for(int i = 0;i< counter;i++){
      printf("%d\n", write(currentClients[i], sendStr,sizeof(sendStr)));
      sendPlayerInit(currentClients[i], gameStarter);
        sendIntToClient(currentClients[i], i);
     }
    



    

     
   int roundCount = 0;
    int fdMax = -1;
    int finalBattleWon = 0;
    int readyForEvents = 0;
    char eventMessage[256];
    
    struct player_data datalist[counter];
    while(!finalBattleWon){
         int playersGone = 0;
        while(playersGone < counter){
           
    FD_ZERO(&read_fds);
    
    for (int i = 0; i<counter;i++){
            FD_SET(currentClients[i], &read_fds);
            if(currentClients[i] > fdMax){
            fdMax = currentClients[i];
        }
        }
    
    int ready_fds = select(fdMax + 1, &read_fds, NULL, NULL, NULL);
    if (ready_fds == -1) {
        perror("select");
        // Handle error...
        break;
    }

    printf("Number of ready file descriptors: %d\n", ready_fds);

    // Process sockets that are ready
    for (int i = 0; i < counter; i++) {
        if (FD_ISSET(currentClients[i], &read_fds)) {
            receivePlayerData(currentClients[i], &datalist[i]);
            printf("Identifier is %d\n", datalist[i].selfIdentifier);
            fflush(stdout);
            playersGone += 1;
           
        }
    }
        }
        printf("turn ended");
        fflush(stdout);
    for (int i = 0; i < counter; i++) {
        if(datalist[i].succeededInFinalBattle){
            finalBattleWon = 1;
        }
        if(datalist[i].gold < 0){
            datalist[i].gold = 0;
        }

        if(datalist[i].population < 0){
            datalist[i].population = 0;
        }

        if(datalist[i].troops < 0){
            datalist[i].troops = 0;
        }
       
        if(datalist[i].duelRequestMade){
            printf("%d%d");
            duel(&datalist[i],&datalist[datalist[i].enemyIdentifier]);
        }
        
        tribeLevel(&datalist[i], eventMessage,difficulty);
       
        if(sendPlayerDataList(currentClients[i], &datalist[0], counter) == -1){
            printf("failed0");
        }
      
        printf("%s\n", eventMessage);
        if(sendStringToClient(currentClients[i], eventMessage) == -1){
         printf("failed0");
        }
   
    }
   

   
    
    //go through all and get new player data, if duel run duel method, if final battle won run get final battle, check for event threshold >2000 soldiers in any, run events, send event data and stuff to players;
    
    
        }
    
    free(hints);
    freeaddrinfo(results);
    return 0;


}



//select -> create sockets for each player -> up to num of players
//start game pulling faction/talent data from players
//send all players starting gold, troops, and cities(based on diffuclty, factions, and talents)
//have a list of player talents/factions/city count/troop count/gold count(list of structs)
//each turn ends when all the sockets have smth in end them]\
//each turn begins by sending out npc challenges, overall challenges
//as turn goes, scanes for challenge intializations and completions
