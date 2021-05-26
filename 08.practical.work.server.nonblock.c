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
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    sockfd=socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
      printf("Error creating socket\n");
      exit(1);
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);
    
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) <0){
        printf("Error creating socket\n");
        exit(1);
    }
//reuse address
    setsockopt(sockfd, SOL_SOCKET,
    SO_REUSEADDR, &(int){ 1 },
    sizeof(int));

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
    }

    printf("Client accepted\n");

    while (1) {
        clientfd = accept(sockfd, (struct sockaddr *) &caddr, &clen);
		if (clientfd > 0) {
            int fl = fcntl(clientfd, F_GETFL, 0);
            fl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, fl);
            printf("Successfully accepted a client\n");
            while (1) {
                char buffer[1024];
                char buffer2[1000];
                const char delim = '\n';
                for (int i = 0; i < 1024; i++) buffer[i]=0;

                printf("Server> ");
                fgets(buffer, 1024, stdin);
                write(clientfd, buffer, strlen(buffer));

                ssize_t len = 1;
                bzero(buffer2,sizeof(buffer2));
                while (buffer2[len-1] != delim){
                    len = read(clientfd, buffer2, 1000);
                    if (len > 0){
                        strncat(buffer, buffer2, len);
                    }
                bzero(buffer2, sizeof(buffer2));
                }

                struct pollfd input[1] = {{.fd = 0, .events = POLLIN}};
                if (poll(input, 1, 100) > 0) {
                    fgets(buffer, 1000, stdin);
                    write(clientfd, buffer, strlen(buffer));
                }
                if (strcmp(buffer, "/dc") == 0) {
                    shutdown(clientfd, SHUT_RDWR);
                    close(clientfd);
                    printf("!!! SERVER DISCONNECTED \n");
                    break;
                }
            }
        }  
    }
    return 0;
}
