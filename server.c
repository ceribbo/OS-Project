#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>  // htons()
#include <netinet/in.h> // struct sockaddr_in
#include <sys/socket.h>

#include "showcase_util.h"
#include "socket_util.h"


typedef struct handler_args_s {
    int socket_desc;
    struct sockaddr_in* client_addr;
} handler_args_t;


void* connection_handler(void* arg) {
    handler_args_t* args = (handler_args_t*)arg;

    /* We make local copies of the fields from the handler's arguments
     * data structure only to share as much code as possible with the
     * other two versions of the server. In general this is not a good
     * coding practice: using simple indirection is better! */
    int socket_desc = args->socket_desc;
    struct sockaddr_in* client_addr = args->client_addr;

    int ret, recv_bytes;
    char username[30];
    char buf[1024];
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
    //uint16_t client_port = ntohs(client_addr->sin_port); // port number is an unsigned short

    // send welcome message
    sprintf(buf, "Hi! This is a showcase server. Please insert your username (max 9 chars): ");

        /*You are %s talking on port %hu.\nI will send you back whatever"
            " you send me. I will stop if you send me %s :-)\n", client_ip, client_port, quit_command);*/

    msg_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to the socket");
    }
    // read username from client
    while ( (recv_bytes = recv(socket_desc, username, buf_len, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot read from socket");
    }
    username[recv_bytes] = '\0';

    //int username_len = strlen(buf);
    //char username[username_len] = buf;
    //char *username = buf;
    printf("CLIENT = %s - HAS JUST CONNECTED\n", username);

    //send instructions to client
    sprintf(buf, "Hi %s! Here it is the showcase:\n",username);
    msg_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to the socket");
    }

    //send first showcase
    send_showcase(socket_desc);
    // echo loop
    while (1) {
        // read message from client
        while ( (recv_bytes = recv(socket_desc, buf, buf_len, 0)) < 0 ) {
            if (errno == EINTR) continue;
            ERROR_HELPER(-1, "Cannot read from socket");
        }

        // check whether I have been ask for the showcase
        if (recv_bytes == show_command_len && !memcmp(buf, show_command, show_command_len)) {
            send_showcase(socket_desc);
        }
        // check whether I have been ask for the showcase
        else if (recv_bytes == new_command_len && !memcmp(buf, new_command, new_command_len)) {
            recv_post(socket_desc, username);
        }
        // check whether I have been ask for the showcase
        else if (recv_bytes == delete_command_len && !memcmp(buf, delete_command, delete_command_len)) {
            recv_delete(socket_desc, username);
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
                ERROR_HELPER(-1, "Cannot write to the socket");
            }
        }

        // ... or if I have to send the message back
        
    }

    // close socket
    ret = close(socket_desc);
    ERROR_HELPER(ret, "Cannot close socket for incoming connection");
    
    if (DEBUG) fprintf(stderr, "Thread created to handle the request has completed.\n");

    free(args->client_addr); // do not forget to free this buffer!
    free(args);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    int ret;
    sem_close(semaphore);
    sem_unlink(SEM_NAME);
    // Initialize a semaphore 
    if ((semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1)) == SEM_FAILED ) {
        printf("Error: semaphore not opened\n");
        exit(EXIT_FAILURE);
    }

    //test***********************************
    insert_post("edu", "oggettoEDU", "testoEDU", "password"); 
    insert_post("ceribbo", "oggettoCE", "testoCE", "password");
    //fineTest*******************************
    int socket_desc, client_desc;

    // some fields are required to be filled with 0
    struct sockaddr_in server_addr = {0};

    int sockaddr_len = sizeof(struct sockaddr_in); // we will reuse it for accept()

    // initialize socket for listening
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    ERROR_HELPER(socket_desc, "Could not create socket");

    server_addr.sin_addr.s_addr = INADDR_ANY; // we want to accept connections from any interface
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(SERVER_PORT); // don't forget about network byte order!

    /* We enable SO_REUSEADDR to quickly restart our server after a crash:
     * for more details, read about the TIME_WAIT state in the TCP protocol */
    int reuseaddr_opt = 1;
    ret = setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt));
    ERROR_HELPER(ret, "Cannot set SO_REUSEADDR option");

    // bind address to socket
    ret = bind(socket_desc, (struct sockaddr*) &server_addr, sockaddr_len);
    ERROR_HELPER(ret, "Cannot bind address to socket");

    // start listening
    ret = listen(socket_desc, MAX_CONN_QUEUE);
    ERROR_HELPER(ret, "Cannot listen on socket");

    // we allocate client_addr dynamically and initialize it to zero
    struct sockaddr_in* client_addr = calloc(1, sizeof(struct sockaddr_in));

    // loop to manage incoming connections spawning handler threads
    while (1) {
        // accept incoming connection
        client_desc = accept(socket_desc, (struct sockaddr*) client_addr, (socklen_t*) &sockaddr_len);
        if (client_desc == -1 && errno == EINTR) continue; // check for interruption by signals
        ERROR_HELPER(client_desc, "Cannot open socket for incoming connection");

        pthread_t thread;
        handler_args_t* thread_args = malloc(sizeof(handler_args_t));
        thread_args->socket_desc = client_desc;
        thread_args->client_addr = client_addr;
        
        if (pthread_create(&thread, NULL, connection_handler, (void*)thread_args) != 0) {
            fprintf(stderr, "Can't create a new thread, error %d\n", errno);
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread);
        client_addr = calloc(1, sizeof(struct sockaddr_in));
    }

    if (sem_close(semaphore) == -1) {
        printf("Error closing the semaphore\n");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEM_NAME) == -1) {
        printf("Error unlinking the semaphore\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS); // this will never be executed
}
