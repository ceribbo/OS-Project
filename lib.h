#ifndef LIB_H
#define LIB_H


#include <semaphore.h>

#include "showcase_util.h"
#include "socket_util.h"


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
#define SERVER_NEW_COMMAND "new"
#define SERVER_DELETE_COMMAND "delete"
#define SERVER_SHOW_COMMAND "show"
#define SERVER_PORT     2187

#endif
