#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // htons() and inet_addr()
#include <netinet/in.h> // struct sockaddr_in
#include <sys/socket.h>

#include "lib.h"
#include "socket_util.h"


int main(int argc, char* argv[]) {
    int ret;

    //possible commands
    char* quit_command = SERVER_QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    char* new_command = SERVER_NEW_COMMAND;
    size_t new_command_len = strlen(new_command);

    char* delete_command = SERVER_DELETE_COMMAND;
    size_t delete_command_len = strlen(delete_command);

    char* show_command = SERVER_SHOW_COMMAND;
    size_t show_command_len = strlen(show_command);

    // variables for handling a socket
    int socket_desc;
    struct sockaddr_in server_addr = {0}; // some fields are required to be filled with 0

    // create a socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_HELPER(socket_desc, "Could not create socket");

    // set up parameters for the connection
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(SERVER_PORT); // don't forget about network byte order!

    // initiate a connection on the socket
    ret = connect(socket_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
    ERROR_HELPER(ret, "Could not create connection");

    if (DEBUG) fprintf(stderr, "Connection established!\n");

    char buf[1024];
    int buf_len = sizeof(buf);
    int msg_len;

    // display welcome message from server
    while ( (msg_len = recv(socket_desc, buf, buf_len - 1, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot read from socket");
    }
    buf[msg_len] = '\0';
    printf("%s", buf);

    //get and send username
    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
        fprintf(stderr, "Error while reading from stdin, exiting...\n");            
        exit(EXIT_FAILURE);
    }
    msg_len = strlen(buf);
    buf[--msg_len] = '\0'; // remove '\n' from the end of the message

    // send username to server
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // display 2° welcome message with username from server
    while ( (msg_len = recv(socket_desc, buf, buf_len - 1, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot read from socket");
    }
    buf[msg_len] = '\0';
    printf("%s", buf);


    //recv first showcase
    recv_showcase(socket_desc);

    // main loop
    while (1) {
        printf("****************CONSOLE****************\n");
        printf("*Type: \n");
        printf("*-new: to create a new post\n");
        printf("*-delete: to delete one of your posts\n");
        printf("*-show: to show the showcase\n");
        printf("*COMMAND: ");

        /* Read a line from stdin
         *
         * fgets() reads up to sizeof(buf)-1 bytes and on success
         * returns the first argument passed to it. */
        if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
            fprintf(stderr, "Error while reading from stdin, exiting...\n");
            exit(EXIT_FAILURE);
        }

        msg_len = strlen(buf);
        buf[--msg_len] = '\0'; // remove '\n' from the end of the message

        // send message to server
        while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
            if (errno == EINTR) continue;
            ERROR_HELPER(-1, "Cannot write to socket");
        }


        //show command so we'll wait for the server to send it
        if (msg_len == show_command_len && !memcmp(buf, show_command, show_command_len))    {
            recv_showcase(socket_desc);
        }else{

            //new command so we are going to write a new post
            if (msg_len == new_command_len && !memcmp(buf, new_command, new_command_len))    {
                send_post(socket_desc);
            }

            //delete command so we are going to delete a post
            if (msg_len == delete_command_len && !memcmp(buf, delete_command, delete_command_len))    {
                send_delete(socket_desc);
            }

            //quit command so we are exting
            if (msg_len == quit_command_len && !memcmp(buf, quit_command, quit_command_len))    {
                break;
            }

            // read message from server
            while ( (msg_len = recv(socket_desc, buf, buf_len, 0)) < 0 ) {
                if (errno == EINTR) continue;
                ERROR_HELPER(-1, "Cannot read from socket");
            }

            printf("SERVER RESPONSE: %s\n", buf); // no need to insert '\0'
        }
    }


    // close the socket
    ret = close(socket_desc);
    ERROR_HELPER(ret, "Cannot close socket");

    if (DEBUG) fprintf(stderr, "Exiting...\n");

    exit(EXIT_SUCCESS);
}
