#include "thread.h"

//handle thread close
void exit_thread(handler_args_t* args, char username[])  {
    printf("CLIENT = %s - HAS JUST DISCONNECTED\n", username);

    // close client socket
    int ret = close(args->socket_desc);
    if (ret == -1) printf("Error closing %s's client socket\n", username);

    // free thread's memory
    free(args->client_addr);
    free(args);
    pthread_exit(NULL);
}

//function to handle a single connection with a client
void* client_handler(void* arg) {
    handler_args_t* args = (handler_args_t*)arg;

    //initialize socket and other structure
    int socket_desc = args->socket_desc;
    struct sockaddr_in* client_addr = args->client_addr;

    int ret, recv_bytes;
    char username[30];
    char buf[4096];
    size_t buf_len = sizeof(buf);
    size_t msg_len;

    //possible commands
    char* quit_command = SERVER_QUIT_COMMAND;
    size_t quit_command_len = strlen(quit_command);

    char* new_command = SERVER_NEW_COMMAND;
    size_t new_command_len = strlen(new_command);

    char* delete_command = SERVER_DELETE_COMMAND;
    size_t delete_command_len = strlen(delete_command);

    char* show_command = SERVER_SHOW_COMMAND;
    size_t show_command_len = strlen(show_command);

    // parse client IP address and port
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);

    // send welcome message
    sprintf(buf, "Hi! This is a showcase server. Please insert your username (max 15 chars): ");

    msg_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
        if (errno == EINTR) continue;
    }
    if (sock_error(ret)) exit_thread(args, "USER");

    // read username from client
    while ( (recv_bytes = recv(socket_desc, username, buf_len, 0)) < 0 ) {
        if (errno == EINTR) continue;
    }
    if (sock_error(recv_bytes)) exit_thread(args, "USER");
    username[recv_bytes] = '\0';

    printf("CLIENT = %s - HAS JUST CONNECTED\n", username);

    //send instructions to client
    sprintf(buf, "Hi %s! Here it is the showcase:\n",username);
    msg_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
        if (errno == EINTR) continue;
    }
    if (sock_error(ret)) exit_thread(args, username);

    //send first showcase
    if (! send_showcase(socket_desc)) exit_thread(args, username);
    // echo loop
    while (1) {
        // read message from client
        while ( (recv_bytes = recv(socket_desc, buf, buf_len, 0)) < 0 ) {
            if (errno == EINTR) continue;
        }
        if (sock_error(recv_bytes)) break;

        // check whether I have been ask for the showcase
        if (recv_bytes == show_command_len && !memcmp(buf, show_command, show_command_len)) {
            if (! send_showcase(socket_desc)) break;
        }
        // check whether I have been ask for the showcase
        else if (recv_bytes == new_command_len && !memcmp(buf, new_command, new_command_len)) {
            if (! recv_post(socket_desc, username) ) break;
        }
        // check whether I have been ask for the showcase
        else if (recv_bytes == delete_command_len && !memcmp(buf, delete_command, delete_command_len)) {
            if (! recv_delete(socket_desc, username) ) break;
        }
        // check whether I have just been told to quit...
        else if (recv_bytes == quit_command_len && !memcmp(buf, quit_command, quit_command_len))    {
            break;
        }
        //error: command not found
        else {
            strcpy(buf, "COMMAND NOT FOUND\0");
            msg_len = strlen(buf);
            while ( (ret = send(socket_desc, buf, msg_len+1, 0)) < 0 ) {
                if (errno == EINTR) continue;
            }
            if (sock_error(ret)) break;
        }
        
    }
    exit_thread(args, username);
    pthread_exit(NULL);
}