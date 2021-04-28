#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    struct hostent *host;
    if (argc < 2) {
      char hostName[200];
      printf("Enter hostname: ");
      scanf("%s",hostName);
      host = gethostbyname(hostName);
    }
    else {
      host = gethostbyname(argv[1]);
    }

    if (host == NULL) {
       printf("gethostbyname() failed\n");
    } else {
       printf("Resolved IP address of %s:\n", host->h_name);
       unsigned int i=0;
       while ( host -> h_addr_list[i] != NULL) {
          printf( "\t-%s\n ", inet_ntoa( *( struct in_addr*)( host -> h_addr_list[i])));
          i++;
       }
       printf("\n End!\n");
    }

}