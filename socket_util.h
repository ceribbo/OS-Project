#ifndef SOCKET_UTIL_H
#define	SOCKET_UTIL_H

#include "showcase_util.h"
#include "lib.h"


//send the showcase via socket
void send_showcase(int socket_desc);

//recv the showcase via socket
void recv_showcase(int socket_desc);

//send new post from client
void send_post(int socket_desc);

//add new post from server
void recv_post(int socket_desc, char* username);

//add new post from server
void send_delete(int socket_desc);

//add new post from server
void recv_delete(int socket_desc, char* username);

#endif