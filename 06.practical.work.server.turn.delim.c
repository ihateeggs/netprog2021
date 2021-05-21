#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *agrv[]){
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    char buffer[1024];

    if (sockfd < 0) {
        printf("Error creating socket\n");
        exit(1);
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("Error binding\n");
        exit(1);
    }

    if (listen(sockfd, 5) < 0) {
        printf("Error listening\n");
        exit(1);
    }

    clen=sizeof(caddr);
    if ((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) < 0) {
        printf("Error accepting connection\n");
        exit(1);
    }else{}

    printf("\t \t------Client accepted!------\n\n");
    printf("\t \tPlease start first!\n");

    while (1) {
        char buffer[1024];
        const char delim = '\n';
        for (int i = 0; i < 1024; i++) buffer[i]=0;
        printf("Server> ");
        bzero(buffer,1024);

        int j = 0;
        j = read(sockfd, buffer, 1023);

        if (j < 0){
            printf("Reading error.\n");
        }
         printf("Server> ");

        bzero(buffer,1024);
        printf("Client says: %s\n", buffer);
        fgets(buffer, 1024, stdin);
        j = write(sockfd, buffer, strlen(buffer));

        if(j <= 0 && buffer[j-1] == delim){
             printf("Writing error.\n");
         }
    }
    return 1;
}
