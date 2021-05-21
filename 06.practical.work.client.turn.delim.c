#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char const *agrv[]){
    struct hostent *host;
    if (argc < 2) {
        char hostName[200];
        printf("Enter hostname: ");
        scanf("%s",hostName);
        host = gethostbyname(hostName);
    }
    else {
        host = gethostbyname(agrv[1]);
    }

    if (host == NULL) {
        printf("gethostbyname() failed\n");
    } 
    else {
        printf("Resolved IP address of %s:\n", host->h_name);
        unsigned int i=0;
        while ( host -> h_addr_list[i] != NULL) {
            printf( "\t-%s\n ", inet_ntoa( *( struct in_addr*)( host -> h_addr_list[i])));
        i++;
    }
    printf("\n");
    }

    int sockfd;
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) <0){
        printf("Error creating socket\n");
        exit(1);
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *) &saddr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    saddr.sin_port = htons(8784);
    if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("Cannot connect\n");
        perror("Connect\n");
    }
    printf("\t \t------Successfully connect to the server!------\n\n");
    printf("\t \tPlease wait for the server to send the message first\n");
    
    while (1) {
        char buffer[1024];
        const char delim = '\n';

        printf("Client> ");
        bzero(buffer,1024);
        fgets(buffer, 1024, stdin);
        int j = 0;
        j = write(sockfd, buffer, strlen(buffer));
        strncat(buffer,sizeof(j),1023);

        for (int i = 0; i < 1024; i++) buffer[i]=0;
        if (j < 0){
            printf("Writing error.");
        }

        bzero(buffer,1024);
        j = read(sockfd, buffer, 1023);

        if(j <= 0 && buffer[j-1] == delim){
             printf("Reading error.");
         }

        printf("Server says: %s\n", buffer);
    }
        return 1;
}