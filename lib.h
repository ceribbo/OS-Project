#ifndef LIB_H
#define LIB_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/socket.h>
#include <semaphore.h>
#include <ctype.h>


//client thread structure
typedef struct handler_args_s {
    int socket_desc;
    struct sockaddr_in* client_addr;
} handler_args_t;

#include "showcase_util.h"
#include "socket_util.h"
#include "thread.h"

// macro to simplify error handling
#define ERROR_HELPER(ret, message)  do {                                \
            if (ret < 0) {                                              \
                fprintf(stderr, "%s: %s\n", message, strerror(errno));  \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        } while (0)

/* Configuration parameters */
#define DEBUG           1   // display debug messages
#define MAX_CONN_QUEUE  3   // max number of connections the server can queue
#define SERVER_ADDRESS  "127.0.0.1"
#define SERVER_QUIT_COMMAND  "QUIT"
#define SERVER_NEW_COMMAND "NEW"
#define SERVER_DELETE_COMMAND "DELETE"
#define SERVER_SHOW_COMMAND "SHOW"
#define SERVER_PORT     2187
#define SEM_NAME "semaphore"

#endif
