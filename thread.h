#ifndef THREAD_H
#define	THREAD_H

#include "lib.h"

//handle thread close
void exit_thread(handler_args_t* args, char username[]);

//function to handle a single connection with a client
void* client_handler(void* arg);

#endif