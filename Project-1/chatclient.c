/*
Jon-Eric Cook
CS-372
Project #1
This program demonstrates a client server setup. The server is run with python code 
and the client is run with C code. Both use the sockets API to talk to each through
a TCP connection. The server will be running, waiting for incoming connections. A 
client will connect to the server and send the first message. The server will then
respond. The client and server will go back and forth in this manner until either
the server or client send "\quit". When the server enters "\quit", it simply closes
the connection and then goes back to listening for the next client. When the client 
enters "quit", it will close the clients connection and end the program. The server 
needs to be terminated with a CTRL+C.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// creates file descriptor, portnumber variable and
// variable to hold returned value from read() ad write()
int sockfd, portno, n;

// serv_addr will contain the address of the server
struct sockaddr_in serv_addr;

// server is a pointer to a structure of type hostent
struct hostent *server;

// complete message to be sent
char message[501];

// holds user input
char buffer[489];

// holds user's handle
char handle[11];

// controls while loop
int d = 1;

// displays message about error and aborts program
void error(const char *msg) {
    perror(msg);
    exit(0);
}

// makes the initial connection to the server
void InitialConnection(char *hn,int pn) {
    // sets the port number from the users input
    portno = pn;
    
    // creates the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // checks if the socket was created
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // gets information about the host
    server = gethostbyname(hn);

    // checks to make sure the host is valid
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // clears serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // sets the fields in serv_addr
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);

    // converts port number to network byte order
    serv_addr.sin_port = htons(portno);

    // connects to the server and checks if it was successful
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
}

// gets the users message and sends it to the server
void SendMessage() {
    // presents the user's handle and waits for message
    printf("%s: ",handle);
    bzero(buffer,489);
    fgets(buffer,488,stdin);

    // removes newline at the end
    int v;
    for (v = 0; v < 489; v++) {
        if (buffer[v] == '\n') {
            buffer[v] = '\0';
        }
    }

    // checks if the user typed "\quit"
    if (buffer[0] == '\\' &&
        buffer[1] == 'q' &&
        buffer[2] == 'u' &&
        buffer[3] == 'i' &&
        buffer[4] == 't') {
            // if so, set d to 0 so the while loop ends
            d = 0;

            // send "\quit" to the server
            n = write(sockfd,buffer,strlen(buffer));

            // check if the write was successful
            if (n < 0) {
                error("ERROR writing to socket");
            }

            // close the connection
            close(sockfd);
    }
    else {
        // clear the message array
        bzero(message,501);

        // put the user handle in it
        strcat(message,handle);

        // put the colon and space init
        strcat(message,": ");

        // put the user's message
        strcat(message,buffer);

        // write the entire message to the server
        n = write(sockfd,message,strlen(message));

        // check if the write was successful
        if (n < 0) {
            error("ERROR writing to socket");
        }
    }
}

// receives the server's message and displays it to the user
void ReceiveMessage() {
    // clear the message array
    bzero(message,501);

    // read from the server
    n = read(sockfd,message,500);

    // check if the read was successful
    if (n < 0) {
        error("ERROR reading from socket");
    }

    // check if the server sent "\quit"
    if (message[6] == '\\' &&
        message[7] == 'q' &&
        message[8] == 'u' &&
        message[9] == 'i' &&
        message[10] == 't') {

            // if so, end the while loop
            d = 0;

            // close the connection
            close(sockfd);
    }
    else {

        // write the message from the server
        printf("%s\n",message);
    }
}

// main program that talks back and forth the the server
int main(int argc, char *argv[]) {

    // checks if the user input the correct number of arguments
    if (argc < 3) {
       fprintf(stderr,"Expected format => %s hostname port\n", argv[0]);
       exit(0);
    }

    // clears the command line
    printf("\e[1;1H\e[2J");

    // clears the handle array and gets the users input
    bzero(handle,10);
    printf("User handle: ");
    fgets(handle,10,stdin);

    // removes the newline from the end
    int v;
    for (v = 0; v < 11; v++) {
        if (handle[v] == '\n') {
            handle[v] = '\0';
        }
    }

    // makes the initial contact with the server
    InitialConnection(argv[1],atoi(argv[2]));

    // clears the command line
    printf("\e[1;1H\e[2J");

    // prints -Messages-
    printf("-Messages-\n");

    // gets the user's message, sends it to the server,
    // waits for a reply and repeats until \quit is typed
    // or sent from the server
    while (d == 1) {
        if (d == 1) {
            // send a message to the server
            SendMessage();
        }
        if (d == 1) {
            // receive a message from the server
            ReceiveMessage();
        }
    }
    return 0;
}

/*
-RESOURCE-
The resource below assisted me in completing this project.
http://www.linuxhowtos.org/C_C++/socket.htm
*/
