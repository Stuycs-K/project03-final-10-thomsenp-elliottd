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
void removeElement(int arr[], int *size, int elementToRemove) {
    int i, j;
    int newSize = *size;

    for (i = 0; i < newSize; i++) {
        if (arr[i] == elementToRemove) {
            // Shift elements to the left to overwrite the element to be removed
            for (j = i; j < newSize - 1; j++) {
                arr[j] = arr[j + 1];
            }
            newSize--; // Decrease the size of the array
            i--; // Recheck the current index since elements have shifted
        }
    }

    *size = newSize; // Update the size of the array
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
    int currentClients[PLAYERCOUNT] = {0};
    fd_set read_fds;
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

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
        printf("got to here\n");
        fflush(stdout);
        for (int i = 0; i < counter; i++) {
            if (currentClients[i] != -1) {
                FD_SET(currentClients[i], &read_fds);
                if (currentClients[i] > fdMax) {
                    fdMax = currentClients[i];
                }
            }
        }
    
        int i = select(fdMax + 1, &read_fds, NULL, NULL, NULL);
        if (i < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        printf("got to here\n");
        fflush(stdout);
        if(counter >= PLAYERCOUNT){
            notStarting = 0;
            printf("WORKING\n");
            fflush(stdout);
        }
        

        else{

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
        else if (FD_ISSET(listen_socket, &read_fds)) {
            
            fflush(stdout);
            //accept the connection
            int connectingClient = accept(listen_socket,(struct sockaddr *)&client_address, &sock_size);
            currentClients[counter++] = connectingClient;
            printf("Connected, to client %d\n", counter);
            fflush(stdout);
            
            
        }

         
        

        else{
            char clientString[256];
        for(int i = 0;i< counter;i++){
           
             if (FD_ISSET(currentClients[i], &read_fds)) {
                if(read(currentClients[i], clientString,255) != 0){
                if(!strcmp(clientString, "start game")){
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
                else{
                    printf("%s\n", clientString);
                }
             }
             }
            

        }
        }
    }

     for(int i = 0;i< counter;i++){
      write(currentClients[i], "1",2);
     }
   int roundCount = 0;
    fdMax = -1;
    FD_ZERO(&read_fds);
    FD_SET(listen_socket, &read_fds);
    if(listen_socket > fdMax){
            fdMax = listen_socket;
    }
    FD_SET(STDIN_FILENO, &read_fds);
    if(STDIN_FILENO > fdMax){
            fdMax = STDIN_FILENO;
        }
    for (int i = 0; i<counter;i++){
            FD_SET(currentClients[i], &read_fds);
            if(currentClients[i] > fdMax){
            fdMax = currentClients[i];
        }
        }
    i = select(fdMax+1, &read_fds, NULL, NULL, NULL);


    free(hints);
    freeaddrinfo(results);
    return 0;
}
}



//select -> create sockets for each player -> up to num of players
//start game pulling faction/talent data from players
//send all players starting gold, troops, and cities(based on diffuclty, factions, and talents)
//have a list of player talents/factions/city count/troop count/gold count(list of structs)
//each turn ends when all the sockets have smth in end them]\
//each turn begins by sending out npc challenges, overall challenges
//as turn goes, scanes for challenge intializations and completions
