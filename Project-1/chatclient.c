#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
       fprintf(stderr,"Expected format => %s hostname port\n", argv[0]);
       exit(0);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char message[501];
    char buffer[489];
    char handle[11];

    bzero(handle,10);
    printf("User handle: ");
    fgets(handle,10,stdin);
    int v;
    for (v = 0; v < 11; v++) {
        if (handle[v] == '\n') {
            handle[v] = '\0';
        }
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    int d = 1;
    while (d == 1) {
        printf("%s: ",handle);
        bzero(buffer,489);
        fgets(buffer,488,stdin);
        for (v = 0; v < 489; v++) {
            if (buffer[v] == '\n') {
                buffer[v] = '\0';
            }
        }

        if (buffer[0] == '\\' &&
            buffer[1] == 'q' &&
            buffer[2] == 'u' &&
            buffer[3] == 'i' &&
            buffer[4] == 't') {

            d = 0;
            n = write(sockfd,buffer,strlen(buffer));
            if (n < 0) {
                error("ERROR writing to socket");
            }
            close(sockfd);
        }
        else {
            bzero(message,501);
            strcat(message,handle);
            strcat(message,": ");
            strcat(message,buffer);
            n = write(sockfd,message,strlen(message));
            if (n < 0) {
                error("ERROR writing to socket");
            }
            bzero(message,501);
            n = read(sockfd,message,500);
            if (n < 0) {
                error("ERROR reading from socket");
            }
            if (message[6] == '\\' &&
                message[7] == 'q' &&
                message[8] == 'u' &&
                message[9] == 'i' &&
                message[10] == 't') {

                d = 0;
                close(sockfd);
            }
            else {
                printf("%s\n",message);
            }
        }
    }
    return 0;
}
