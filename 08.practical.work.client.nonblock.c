#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

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
    //Reuse address
    setsockopt(sockfd, SOL_SOCKET,
        SO_REUSEADDR, &(int){ 1 },
    sizeof(int));

    //Nonblocking
    int fl = fcntl(sockfd, F_GETFL, 0);
	fl |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, fl);

    printf("\t \t------Successfully connect to the server!------\n\n");
    printf("\t \tPlease wait for the server to send the message first\n");
    
    while (1) {
        char buffer[1024];
        char buffer2[1000];
        const char delim = '\n';

        for (int i = 0; i < 1024; i++) buffer[i]=0;
        ssize_t len = 1;
        bzero(buffer2,sizeof(buffer2));
        while (buffer2[len-1] != delim){
            len = read(sockfd, buffer2, 1000);
            if (len > 0){
                strncat(buffer, buffer2, len);
            }
            bzero(buffer2, sizeof(buffer2));
        }

        struct pollfd input[1] = {{.fd = 0, .events = POLLIN}};
        if (poll(input, 1, 100) > 0) {
            fgets(buffer, 1024, stdin);
            write(sockfd, buffer, strlen(buffer));
        }
        if (strcmp(buffer, "/quit") == 0) {
            shutdown(sockfd, SHUT_RDWR);
            close(sockfd);
            exit(0);
        }
    }

        return 0;
}