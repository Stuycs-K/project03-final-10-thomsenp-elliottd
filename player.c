#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(){
    

    char *hostname = "127.0.0.1";
    char* port = "9998";
    struct addrinfo hints, *server_info;

    // Initialize hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &server_info);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    int client_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (client_socket == -1) {
        perror("socket");
        exit(1);
    }

    if (connect(client_socket, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        perror("connect");
        close(client_socket);
        exit(1);
    }

    char buff[1025];
    buff[0] = '\0'; // Initialize the first element to null character
    printf("Connected to server. Type 'start game' to begin.\n");

    while (1) {
        // memset(buff, 0, sizeof(buff)); // No need to use memset here

        fgets(buff, sizeof(buff) - 1, stdin);
        buff[strlen(buff) - 1] = '\0'; // Remove newline character

       if (write(client_socket, buff, strlen(buff)) == -1) {
            perror("write");
            close(client_socket);
            exit(1);
        }

        if (strcmp(buff, "start game") == 0) {
            break; // Exit the loop after sending "start game"
        }
    }

    close(client_socket);
    printf("Game starting...\n");

    return 0;
}
// int main(){
//     char currentInput[100];
//     int turnNum = 0;

//     int client_tcp_handshake(char * server_address) {
//     struct addrinfo * hints, * results;
//     hints = calloc(1,sizeof(struct addrinfo));
//     hints->ai_family = AF_INET;
//     hints->ai_socktype = SOCK_STREAM; //TCP socket
//     getaddrinfo(server_address, PORT, hints, &results);
  
//     int serverd = socket(AF_INET, SOCK_STREAM,0);
//     connect(serverd, results->ai_addr, results->ai_addrlen);
//   //connect to the server
  
//   free(hints);
//   freeaddrinfo(results);

//   return serverd;
// }
//     while(1){
//         printf("Turn:%d    Type 's' to see current stats, 'r' to see and recruit troops, 'c' to see and take on challenges and duels, and 'b' to see and build buildings\n", turnNum);
//         scanf("%99s", currentInput); // Reads a string from stdin until a space is encountered
        
        

//         if(strcmp(currentInput, "s") == 0){
//             printf("Gold = PLACEHOLDER\nPopulation = PLACEHOLDER\nGold per Turn = PLACEHOLDER\nPopulation per Turn = PLACEHOLDER\nFaction = PLACEHOLDER\nCurrent Talents = PLACEHOLDER\nGlobal Gold Ranking = PLACEHOLDER LIST\nGlobal Military Ranking = PLACEHOLDER LIST\n");
//             turnNum++;
//         }

//         else if(strcmp(currentInput, "r") == 0){
//             printf("Current Troops = PLACEHOLDER LIST WITH NUMS AND STATS\nTroops Avaliable\n type '0 numOfTroops'(Ex: 10) to select TROOP 1: PRICE\nSTATS = PLACEHOLDER LIST\nREPEAT AS NEEDED\n");
//             printf("Desired troop and number('x' to cancel)");
//             scanf("%99s", currentInput);
//             //will not work as is, based on spaces
//             printf("troopTest%s", currentInput);
//             //scan to get int, run through list
//             turnNum++;
            
//         }

//         else if(strcmp(currentInput, "c") == 0){
//             printf("Turn Challenge = PLACEHOLDER\nOngoging Server Challenge = PLACEHOLDER\nPlayer List = PLACEHOLDER LIST\nFINAL BATTLE REQUIREMENTS = PLACEHOLDER\n");
//             printf("Desired menu('x' to cancel)");
//             scanf("%99s", currentInput);
//             printf("challengeTest%s", currentInput);
//             turnNum++;
            
//         }

//         else if(strcmp(currentInput, "b") == 0){
//             printf("Current Buildings = PLACEHOLDER LIST\nBuildings avaliable = LIST WITH PRICES AND TIME\n");
//             printf("Desired building('x' to cancel)");
//             scanf("%99s", currentInput);
//             printf("buildingTest%s", currentInput);
//             turnNum++;
            
//         }

//         else{
//             printf("Invalid input, please type 's' to see current stats, 'r' to see and recruit troops, 'c' to see and take on challenges and duels, and 'b' to see and build buildings\n");
//         }
//     }
    
// }



