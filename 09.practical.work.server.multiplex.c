#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/select.h>
#define MAX_CLIENT 100

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
//reuse 
    int clientfds[100]; // list of connected clients, >0 if valid
    memset(clientfds, 0, sizeof(clientfds));
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

    while (1) {
        fd_set set; // declaration of the set
        FD_ZERO(&set); // clear the set
        FD_SET(sockfd, &set); // add listening sockfd to set
        int maxfd = sockfd; // a required value to pass to select()
        for (int i = 0; i < 100; i++) {
// add connected client sockets to set
            if (clientfds[i] > 0) FD_SET(clientfds[i], &set);
            if (clientfds[i] > maxfd) maxfd = clientfds[i];
        }
// poll and wait, blocked indefinitely
        select(maxfd+1, &set, NULL, NULL, NULL);
// each time we accept, we add client socket to clientfds[] array
        if (FD_ISSET(sockfd, &set)) {
            clientfd = accept(sockfd, (struct sockaddr *) &saddr, &clen);
// make it nonblocking
            int fl = fcntl(clientfd, F_GETFL, 0);
            fl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, fl);
// add it to the clientfds array
            for (int i = 0; i < MAX_CLIENT; i++) {
                if (clientfds[i] == 0) {
                clientfds[i] = clientfd;
                break;
                }   
            }       
    }

    clen=sizeof(caddr);
    if ((clientfd < 0)) {
        printf("Error accepting connection\n");
        exit(1);
    }

    printf("\t \tClient accepted\n");

    while (1) {
		clientfd = accept(sockfd, (struct sockaddr *) &caddr, &clen);
		if (clientfd > 0) {
			printf("Successfully accepted a client\n");
			while (1) {
				char s[1000];
				memset(s, 0, 1000);
				if (read(clientfd, s, 1000) > 0) {
					printf("Client says: %s\n", s);
				}
				struct pollfd input[1] = {{.fd = 0, .events = POLLIN}};
				if (poll(input, 1, 100) > 0) {
		      		printf("Server> ");
      	            scanf(s, 1000, stdin);
			      	write(clientfd, s, strlen(s));
			    }
			}
		}
    }
    return 0;
    }
}