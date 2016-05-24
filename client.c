#include "lib.h"

int client_socket_desc, ret;

void exit_client()  {
    printf("\nEXITING FROM CLIENT...\n");

    // close server socket
    ret = close(client_socket_desc);
    ERROR_HELPER(ret, "Cannot close socket while server shutting down");

    // this should never be executed
    exit(EXIT_SUCCESS); 
}

int main(int argc, char* argv[]) {
    int ret;

    //set signals to receive
    signal(SIGINT, exit_client);
    signal(SIGTERM, exit_client);
    signal(SIGQUIT, exit_client);
    signal(SIGILL, exit_client);
    signal(SIGHUP, exit_client);
    signal(SIGSEGV, exit_client);
    signal(SIGPIPE, SIG_IGN);

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
    struct sockaddr_in server_addr = {0}; 

    // create a socket
    client_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_HELPER(client_socket_desc, "Could not create socket");

    // set up parameters for the connection
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(SERVER_PORT); 

    // initialize a connection on the socket
    ret = connect(client_socket_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
    ERROR_HELPER(ret, "Could not create connection");

    char buf[4096];
    int buf_len = sizeof(buf);
    int msg_len;

    // display welcome message from server
    while ( (msg_len = recv(client_socket_desc, buf, buf_len - 1, 0)) < 0 ) {
        if (errno == EINTR) continue;
    }
    if (sock_error(msg_len)) exit_client();
    buf[msg_len] = '\0';
    printf("%s", buf);

    while(1)    {
        //get and send username
        if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
            fprintf(stderr, "Error while reading from stdin, exiting...\n");            
            exit_client();
        }
        msg_len = strlen(buf);
        buf[--msg_len] = '\0'; 

        //check if > 9
        if (strlen(buf) > 0 && strlen(buf) <= 15) {
            break;
        }
        printf("Error: too many characters in the username! Try again: ");
    }   

    // send username to server
    while ( (ret = send(client_socket_desc, buf, msg_len, 0)) < 0) {
        if (errno == EINTR) continue;
    }
    if (sock_error(ret)) exit_client();

    // display 2° welcome message with username from server
    while ( (msg_len = recv(client_socket_desc, buf, buf_len - 1, 0)) < 0 ) {
        if (errno == EINTR) continue;
    }
    if (sock_error(msg_len)) exit_client();
    buf[msg_len] = '\0';
    printf("%s", buf);


    //recv first showcase
    if (! recv_showcase(client_socket_desc) ) exit_client();

    // main loop
    while (1) {
        printf("****************CONSOLE****************\n");
        printf("*Type: \n");
        printf("*-%s: to create a new post\n", new_command);
        printf("*-%s: to delete one of your posts\n", delete_command);
        printf("*-%s: to show the showcase\n", show_command);
        printf("*-%s: to quit\n", quit_command);
        printf("*COMMAND: ");

        // get command
        if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
            fprintf(stderr, "Error while reading from stdin, exiting...\n");
            exit(EXIT_FAILURE);
        }

        msg_len = strlen(buf);
        buf[--msg_len] = '\0'; 

        // send command to server
        while ( (ret = send(client_socket_desc, buf, msg_len, 0)) < 0) {
            if (errno == EINTR) continue;
        }
        if (sock_error(ret)) break;


        //show command so we'll wait for the server to send it
        if (msg_len == show_command_len && !memcmp(buf, show_command, show_command_len))    {
            if (! recv_showcase(client_socket_desc)) break;
        }else{

            //new command so we are going to write a new post
            if (msg_len == new_command_len && !memcmp(buf, new_command, new_command_len))    {
                if (! send_post(client_socket_desc)) break;
            }

            //delete command so we are going to delete a post
            if (msg_len == delete_command_len && !memcmp(buf, delete_command, delete_command_len))    {
                if (! send_delete(client_socket_desc)) break;
            }

            //quit command so we are exting
            if (msg_len == quit_command_len && !memcmp(buf, quit_command, quit_command_len))    {
                break;
            }

            // read message from server
            while ( (msg_len = recv(client_socket_desc, buf, buf_len, 0)) < 0 ) {
                if (errno == EINTR) continue;
            }
            if (sock_error(msg_len)) exit_client();

            printf("SERVER RESPONSE: %s\n", buf); // no need to insert '\0'
        }
    }
    exit_client();
}
