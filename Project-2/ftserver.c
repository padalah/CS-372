/*
Jon-Eric Cook
CS-372
Project #2
This program demonstrates a client server setup. The server is being run with C code 
and the client is being run with python code. Both use the sockets API to talk to each
through a TCP connection, both a control connection and also a data connection. The 
server will be running, waiting for incoming connections. A client will connect to the 
server and send a command. There are only two options for commands, "-l" or "-g". When 
the client sends "-l", it is asking the server to send back the contents of the directory
it is in. When the client sends "-g", followed by a file name, it is asking the server to
send back that file. The server will then respond with the corresponding information. If
there is no file, it will send back an error. After a single command from the client, the
client will close the connection and the program will terminate. The server will continue 
to wait and listen for another incoming connection. The server needs to be terminated with 
a CTRL+C.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>

// prints the expected argument format
void print_argument_format() {
    printf("***** General argument format *****\n");
    printf("./ftserver <server-port-number>\n");
    printf("***** Acceptable argument format *****");
    printf("./ftserver 1024->65535");
}

// prints the error, prints the expected argument format and then exits
void validation_error(char *msg) {
    perror(msg);
    print_argument_format();
    exit(1);
}

// prints the error and then exits
void error(char *msg) {
    perror(msg);
    exit(1);
}

// validates the input arguments
void validate_arguments(int arg_count, char *port_number) {

    // checks if there are less than 2 arguments
    if (arg_count < 2) {
        validation_error("ERROR: expected 2 arguments");
    }

    char *p;
    int pn = 0;
    errno = 0;
    long conv = strtol(port_number,&p,10);

    // checks if the port number is an integer
    if (errno != 0 || *p != '\0') {
        validation_error("ERROR: server port number must be an integer");
    } else {
        pn = conv;
    }

    // checks if the port number is within the correct bounds
    if (pn < 1024 || pn > 65535) {
        validation_error("ERROR: invalid port number");
    }

}

// gets the contents of the current directory
// this function is called when the client sends "-l"
int get_directory_contents(char* path[]) {
    DIR *d;
    struct dirent *dir;
    int overall_size = 0;
    int number_of_files = 0;

    // open the current directry
    d = opendir(".");
    if (d) {
        int i = 0;

        // adds the contents to path
        // logs size of content
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                path[i] = dir->d_name;
                overall_size += strlen(path[i]);
                i++;
            }
        }
        number_of_files = i - 1;
    }
    // closes the directory
    closedir(d);

    // returns the sum of overall size and the number of files
    return overall_size + number_of_files;
}

// gets the contents of the passed in file name
char* get_file_contents(char* file_name) {
    char *source = NULL;

    // opens the file
    FILE* fp = fopen(file_name, "r");

    // confirms if it can be opened
    if (fp == NULL) {
        error("ERROR: unable to open file");
    }

    // if the the file was opened
    if (fp != NULL) {
        
        // start at the beginning of the file
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long buffer_size = ftell(fp);

            // confirms the file was valid
            if (buffer_size == -1) {
                error("ERROR: invalid file");
            }

            // creats the variabl to hold the files contents
            source = malloc(sizeof(char) * (buffer_size + 1));

            // confirms can start at beginning
            if (fseek(fp, 0L, SEEK_SET) != 0) {
                error("ERROR: unable to read file");
            }

            // reads contents into source
            size_t new_length = fread(source, sizeof(char), buffer_size, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("ERROR: error while reading file", stderr);
            } else {
                source[new_length++] = '\0';
            }
        }
    }
    
    // closes the file
    fclose(fp);

    // returns the contents of the file
    return source;
}

// sends a message to the client
void send_message_to_client(int sock, char* buffer) {
    ssize_t n;
    size_t size = strlen(buffer) + 1;
    size_t total = 0;

    // sends data until all is sent
    while (total < size) {
        n = write(sock, buffer, size - total);

        total += n;

        // checks if there was an error with the write
        if (n < 0) {
            error("ERROR: unable to send a message");
        }

        // increases the total amount sent
        else if (n == 0) {
            total = size - total;
        }
    }
}

// sends a number to the client
void send_number_to_client(int sock, int numb) {
    ssize_t n = 0;

    // sends a number to the client
    n = write(sock, &numb, sizeof(int));

    // checks if there was an error with the write
    if (n < 0) {
        error("ERROR: unable to send number to client");
    }
}

// sends the file to the client
void send_file_to_client(int sock, char* file_name) {
    char* contents_to_send;
    contents_to_send = get_file_contents(file_name);

    // sends the length of the contents to be sent
    send_number_to_client(sock, strlen(contents_to_send));

    // sends the contents
    send_message_to_client(sock, contents_to_send);
}

// receives a message from the client
void receive_message_from_client(int sock, char* buffer, size_t size) {
    char temp_buffer[size + 1];
    ssize_t n;
    size_t total = 0;

    // reads from the client until all has been received
    while (total < size) {
        n = read(sock, temp_buffer + total, size - total);
        total += n;

        // checks if there was an error with the read
        if (n < 0){
            error("ERROR: unable to receive a message");
        }
    }
    strncpy(buffer, temp_buffer, size);
}

// recieves a number from the client
int receive_number_from_client(int sock) {
    int numb;
    ssize_t n = 0;

    // reads the number from the client
    n = read(sock, &numb, sizeof(int));

    // checks if there was an error with the read
    if (n < 0) {
        error("ERROR: unable to receive number from client");
    }

    // returns the number
    return numb;
}

// handles the request the from the client
int handle_request_from_client(int sock, int* data_port_numb) {
    char command[3] = "\0";

    // receives the message from the client
    receive_message_from_client(sock, command, 3);
    *data_port_numb = receive_number_from_client(sock);

    // checks which command was sent
    if (strcmp(command, "-l") == 0) {
        return 1;
    }
    if (strcmp(command, "-g") == 0) {
        return 2;
    }

    return 0;
}

// starts up a server
int startup_a_server(int port_number) {
    int sockfd;

    // makes sure the new socket worked
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }

    // sets the server info
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port_number);
    server.sin_addr.s_addr = INADDR_ANY;
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    
    // binds the server
    if(bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        error("ERROR: unable to bind");
    }

    // sets the server to listen
    if(listen(sockfd, 10) < 0){
        error("ERROR: unable to listen");
    }

    return sockfd;
}


// main code of ftserver.c
int main(int argc, char *argv[]) {

    // socket variables
    int sockfd, newsockfd, datasockfd, server_port_number, pid;

    // validate user arguments
    validate_arguments(argc,argv[1]);

    // convert the server port argument to a number
    server_port_number = atoi(argv[1]);

    // start up a server
    sockfd = startup_a_server(server_port_number);
    printf("Server open on %d\n", server_port_number);

    // loops until told to stop by a CTRL+C
    while(1) {
        
        // accept connection
        newsockfd = accept(sockfd, NULL, NULL);

        // confirm the connection worked
        if(newsockfd < 0) {
            error("ERROR: unable to accept\n");
        }

        // creat a child process
        pid = fork();

        // confirm the fork worked
        if (pid < 0) {
            error("ERROR: problem with fork\n");
        }

        // only proceed if the fork worked
        if (pid == 0) {
            close(sockfd);
            int command = 0;
            int data_port_number;
            int newsock;

            printf("Connection from flip2.\n");

            // handle the request from the client
            command = handle_request_from_client(newsockfd, &data_port_number);

            // check if the command was correct
            if (command == 0) {
                error("ERROR: expecting -l or -g");
            }

            // check if the command was a "-l"
            if (command == 1) {
                char* path[100];
                int i = 0;
                int length = 0;
                printf("List directory requested on port %d.\n", data_port_number);

                // gets the directory contents
                length = get_directory_contents(path);

                // start up a server
                newsock = startup_a_server(data_port_number);

                // accept a connection
                datasockfd = accept(newsock, NULL, NULL);

                // confirm socket worked
                if (datasockfd < 0) {
                    error("ERROR: unable to open data socket");
                }

                // send the length of the directory contents to the client
                send_number_to_client(datasockfd, length);

                printf("Sending directory contents to flip2: %d\n", data_port_number);

                // send the directory contents
                while (path[i] != NULL) {
                    send_message_to_client(datasockfd, path[i]);
                    i++;
                }

                // close the sockets and exit
                close(newsock);
                close(datasockfd);
                exit(0);
            }

            // checks if the command was "-g"
            if (command == 2) {

                // gets the sent number from the client
                int i = receive_number_from_client(newsockfd);
                char file_name[255] = "\0";

                // gets the file name from the client
                receive_message_from_client(newsockfd, file_name, i);
                printf("File \"%s\" requested on port %d.\n", file_name, data_port_number);

                // checks if the file can be found
                if (access(file_name, F_OK) == -1) {
                    printf("File not found. Sending error message to flip2: %d\n", server_port_number);
                    char error_message[] = "FILE NOT FOUND";

                    // sends the error message length to the client
                    send_number_to_client(newsockfd, strlen(error_message));

                    // sends the error message to the client
                    send_message_to_client(newsockfd, error_message);

                    // close the sockets and exits
                    close(newsock);
                    close(datasockfd);
                    exit(1);
                }
                else {
                    // indicates the file was found
                    char message[] = "FILE FOUND";

                    // send back the length of the message
                    send_number_to_client(newsockfd, strlen(message));

                    // send back the message
                    send_message_to_client(newsockfd, message);
                }
                printf("Sending \"%s\" to flip2: %d\n", file_name, data_port_number);

                // start up a server
                newsock = startup_a_server(data_port_number);
                datasockfd = accept(newsock, NULL, NULL);

                // confirms the socket worked
                if (datasockfd < 0) {
                    error("ERROR: unable to open data socket");
                }

                // send the file to the client
                send_file_to_client(datasockfd, file_name);

                // close sockets and exits
                close(newsock);
                close(datasockfd);
                exit(0);
            }
            exit(0);
        }

    }
}

/*
-RESOURCES-
The resources below assisted me in completing this project.
ftp://gaia.cs.umass.edu/pub/kurose/ftpserver.c
https://github.com/gregmankes/cs372-project2/blob/master/ftserver.c
https://beej.us/guide/bgnet/output/html/multipage/index.html
https://github.com/mustang25/CS372/blob/master/Project2/ftserver.c
https://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleserver
https://forgetcode.com/C/1201-File-Transfer-Using-TCP
https://codereview.stackexchange.com/questions/43914/client-server-implementation-in-c-sending-data-files
https://github.com/fedackb/ftp-server/blob/master/ftserver.c
https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int
http://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
https://github.com/smithg6/cs372-project2/blob/master/ftserver.c
http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
http://www.linuxhowtos.org/data/6/server.c
http://www.linuxhowtos.org/C_C++/socket.htm
*/