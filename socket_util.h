#ifndef SOCKET_UTIL_H
#define	SOCKET_UTIL_H

#include "lib.h"

//check if error from socket
int sock_error(int sock_bytes);

//send the showcase via socket
int send_showcase(int socket_desc);

//recv the showcase via socket
int recv_showcase(int socket_desc);

//send new post from client
int send_post(int socket_desc);

//add new post from server
int recv_post(int socket_desc, char* username);

//add new post from server
int send_delete(int socket_desc);

//add new post from server
int recv_delete(int socket_desc, char* username);

#endif