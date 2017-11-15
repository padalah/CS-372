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

void print_argument_format() {
    printf("***** General argument format *****\n");
    printf("./ftserver <server-port-number>\n");
    printf("***** Acceptable argument format *****");
    printf("./ftserver 1024->65535");
}

void validation_error(char *msg) {
    perror(msg);
    print_argument_format();
    exit(1);
}

void error(char *msg) {
    perror(msg);
    exit(1);
}

void validate_arguments(int arg_count, char *port_number) {

    if (arg_count < 2) {
        validation_error("ERROR: expected 2 arguments");
    }

    char *p;
    int pn = 0;
    errno = 0;
    long conv = strtol(port_number,&p,10);

    if (errno != 0 || *p != '\0') {
        validation_error("ERROR: server port number must be an integer");
    } else {
        pn = conv;
    }

    if (pn < 1024 || pn > 65535) {
        validation_error("ERROR: invalid port number");
    }

}

int startup_a_server(int port_number) {
    int sockfd;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port_number);
    server.sin_addr.s_addr = INADDR_ANY;

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    if(bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        error("ERROR: unable to bind");
    }

    if(listen(sockfd, 10) < 0){
        error("ERROR: unable to listen");
    }

    return sockfd;
}

void send_message_to_client(int sock, char* buffer) {
    ssize_t n;
    size_t size = strlen(buffer) + 1;
    size_t total = 0;

    while (total < size) {
        n = write(sock, buffer, size - total);

        total += n;

        if (n < 0) {
            error("ERROR: unable to send a message");
        }

        else if (n == 0) {
            total = size - total;
        }
    }
}

void receive_message_from_client(int sock, char* buffer, size_t size) {
    char temp_buffer[size + 1];
    ssize_t n;
    size_t total = 0;

    while (total < size) {
        n = read(sock, temp_buffer + total, size - total);
        total += n;
        if (n < 0){
            error("ERROR: unable to receive a message");
        }
    }
    strncpy(buffer, temp_buffer, size);
}

int get_directory_contents(char* path[]) {
    DIR *d;
    struct dirent *dir;
    int overall_size = 0;
    int number_of_files = 0;

    d = opendir(".");
    if (d) {
        int i = 0;
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                path[i] = dir->d_name;
                overall_size += strlen(path[i]);
                i++;
            }
        }
        number_of_files = i - 1;
    }
    closedir(d);
    return overall_size + number_of_files;
}

char* get_file_contents(char* file_name) {
    char *source = NULL;

    FILE* fp = fopen(file_name, "r");

    if (fp == NULL) {
        error("ERROR: unable to open file");
    }

    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long buffer_size = ftell(fp);
            if (buffer_size == -1) {
                error("ERROR: invalid file");
            }
            source = malloc(sizeof(char) * (buffer_size + 1));

            if (fseek(fp, 0L, SEEK_SET) != 0) {
                error("ERROR: unable to read file");
            }

            size_t new_length = fread(source, sizeof(char), buffer_size, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("ERROR: error while reading file", stderr);
            } else {
                source[new_length++] = '\0';
            }
        }
    }
    fclose(fp);
    return source;
}

int receive_number_from_client(int sock) {
    int numb;
    ssize_t n = 0;
    n = read(sock, &numb, sizeof(int));

    if (n < 0) {
        error("ERROR: unable to receive number from client");
    }
    return numb;
}

void send_number_to_client(int sock, int numb) {
    ssize_t n = 0;

    n = write(sock, &numb, sizeof(int));
    if (n < 0) {
        error("ERROR: unable to send number to client");
    }
}

int handle_request_from_client(int sock, int* data_port_numb) {
    char command[3] = "\0";

    receive_message_from_client(sock, command, 3);
    *data_port_numb = receive_number_from_client(sock);

    if (strcmp(command, "-l") == 0) {
        return 1;
    }

    if (strcmp(command, "-g") == 0) {
        return 2;
    }

    return 0;
}

void send_file_to_client(int sock, char* file_name) {
    char* contents_to_send;
    contents_to_send = get_file_contents(file_name);

    send_number_to_client(sock, strlen(contents_to_send));
    send_message_to_client(sock, contents_to_send);
}

int main(int argc, char *argv[]) {

    int sockfd, newsockfd, datasockfd, server_port_number, pid;

    validate_arguments(argc,argv[1]);

    server_port_number = atoi(argv[1]);

    sockfd = startup_a_server(server_port_number);
    printf("Server open on %d\n", server_port_number);

    while(1) {
        newsockfd = accept(sockfd, NULL, NULL);
        if(newsockfd < 0) {
            error("ERROR: unable to accept\n");
        }
        pid = fork();
        if (pid < 0) {
            error("ERROR: problem with fork\n");
        }

        if (pid == 0) {
            close(sockfd);
            int command = 0;
            int data_port_number;
            int newsock;

            printf("Control connection started on port %d.\n", server_port_number);
            command = handle_request_from_client(newsockfd, &data_port_number);

            if (command == 0) {
                error("ERROR: expecting -l or -g");
            }

            if (command == 1) {
                char* path[100];
                int i = 0;
                int length = 0;
                printf("List directory requested on port %d.\n", data_port_number);
                length = get_directory_contents(path);

                newsock = startup_a_server(data_port_number);
                datasockfd = accept(newsock, NULL, NULL);
                if (datasockfd < 0) {
                    error("ERROR: unable to open data socket");
                }
                send_number_to_client(datasockfd, length);
                while (path[i] != NULL) {
                    send_message_to_client(datasockfd, path[i]);
                    i++;
                }
                close(newsock);
                close(datasockfd);
                exit(0);
            }

            if (command == 2) {
                int i = receive_number_from_client(newsockfd);
                char file_name[255] = "\0";
                receive_message_from_client(newsockfd, file_name, i);
                printf("File \"%s\" requested on port %d\n", file_name, data_port_number);

                if (access(file_name, F_OK) == -1) {
                    printf("File not found. Sending error message on port %d\n", server_port_number);
                    char error_message[] = "FILE NOT FOUND";
                    send_number_to_client(newsockfd, strlen(error_message));
                    send_message_to_client(newsockfd, error_message);
                    close(newsock);
                    close(datasockfd);
                    exit(1);
                }
                else {
                    char message[] = "FILE FOUND";
                    send_number_to_client(newsockfd, strlen(message));
                    send_message_to_client(newsockfd, message);
                }
                printf("Sending \"%s\" on port %d\n", file_name, data_port_number);

                newsock = startup_a_server(data_port_number);
                datasockfd = accept(newsock, NULL, NULL);
                if (datasockfd < 0) {
                    error("ERROR: unable to open data socket");
                }
                send_file_to_client(datasockfd, file_name);
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