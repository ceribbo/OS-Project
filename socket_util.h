#ifndef SOCKET_UTIL_H
#define	SOCKET_UTIL_H

#include "lib.h"

//check if error in socket
int sock_error(int sock_bytes);

//send the showcase via socket server->client
int send_showcase(int socket_desc);

//recv the showcase via socket client<-server
int recv_showcase(int socket_desc);

//send new post via socket client->server
int send_post(int socket_desc);

//receive new post via socket server<-client
int recv_post(int socket_desc, char* username);

//send delete via socket client->server
int send_delete(int socket_desc);

//receive delete via socekt server<-client
int recv_delete(int socket_desc, char* username);

#endif