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
    

    char buff[1025]="";
    int notStarting = 1;
    counter = 0;
    
    while(notStarting){
        

        FD_ZERO(&read_fds);
        FD_SET(listen_socket, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        for (int i = 0; i<counter;i++){
            FD_SET(currentClients[i], &read_fds);
        }
        printf("working up to here 0\n");
        fflush(stdout);
        
        if(counter >= PLAYERCOUNT){
            notStarting = 0;
            printf("WORKING\n");
            fflush(stdout);
        }
        

        else{
        //if standard in, use fgets
          

        // if socket
        if (FD_ISSET(listen_socket, &read_fds)) {
            printf("recieved connections\n");
            fflush(stdout);
            //accept the connection
            int client_socket = accept(listen_socket,(struct sockaddr *)&client_address, &sock_size);
            printf("Connected, waiting for data.\n");

            //read the whole buff
            
            currentClients[counter++] = client_socket;
            

            printf("\nCLient count'%d'\n",counter);
            close(client_socket);
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
        }
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
