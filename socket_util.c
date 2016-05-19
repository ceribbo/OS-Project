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

#include "socket_util.h"

//send the showcase to the client
void send_showcase(int socket_desc)	{
	char buf[1024];
    int msg_len;
    int buf_len = sizeof(buf);
    int ret, recv_bytes;

	if (!empty_showcase())	{
		showcase* punt = bacheca;
		while (punt != NULL) {
			//write post into buffer
			sprintf(buf, "*---------------------------------------\n*Time: %s*Author: %s\n*Text: %s\n*ID: %d\n",
				ctime(&(punt->element.date)), punt->element.author, punt->element.text, punt->element.id);
			//send post
		    msg_len = strlen(buf);
		    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
		        if (errno == EINTR) continue;
		        ERROR_HELPER(-1, "Cannot write to the socket");
		    }

	        // wait for client's ACK  
	        while ( (recv_bytes = recv(socket_desc, buf, 3, 0)) < 0 ) {
	            if (errno == EINTR) continue;
	            ERROR_HELPER(-1, "Cannot read from socket");
	        } 
        	punt = punt->next;
	    }
	}else{
		//the showcase is empty....
		sprintf(buf, "Hi! Unfortunately there are no posts on the showcase.\n");
		msg_len = strlen(buf);
	    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
	        if (errno == EINTR) continue;
	        ERROR_HELPER(-1, "Cannot write to the socket");
	    }
	    // wait for client's ACK  
	    while ( (recv_bytes = recv(socket_desc, buf, buf_len, 0)) < 0 ) {
	        if (errno == EINTR) continue;
	        ERROR_HELPER(-1, "Cannot read from socket");
	    } 
	}
	
	//send finished command
	sprintf(buf,"finished");
	msg_len = strlen(buf);
	while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
		if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to the socket");
	}

	print_showcase();
	return;
}

//receive the showcase from the server
void recv_showcase(int socket_desc)	{
	char buf[1024];
	int msg_len;
    int buf_len = sizeof(buf);
    int ret;

	printf("**************************************************************************************\n");
	while (1)	{

		// read message from server
        while ( (msg_len = recv(socket_desc, buf, buf_len, 0)) < 0 ) {
            if (errno == EINTR) continue;
            ERROR_HELPER(-1, "Cannot read from socket");
        }

        //check if the showcase is finished
        if (msg_len == 8 && !memcmp(buf, "finished", 8)) 	{
        	break;
        }

        //send ACK to server to confirm
		while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
            if (errno == EINTR) continue;
            ERROR_HELPER(-1, "Cannot write to socket");
        }
	    buf[msg_len] = '\0';

	    printf("%s", buf);
	}
	printf("**************************************************************************************\n");
	return;
}

//send new post
void send_post(int socket_desc)	{
	char buf[1024];
	int msg_len = 0;
	int ret;

	//get text for new post
	printf("Insert text of the post: ");

    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
        fprintf(stderr, "Error while reading from stdin, exiting...\n");
        exit(EXIT_FAILURE);
    }

    //send text for new post
    msg_len = strlen(buf);
    buf[--msg_len] = '\0'; // remove '\n' from the end of the message

    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}

	//get password for new post
    printf("Insert password for post: ");

    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
        fprintf(stderr, "Error while reading from stdin, exiting...\n");
        exit(EXIT_FAILURE);
    }

    //send text for new post
    msg_len = strlen(buf);
    buf[--msg_len] = '\0'; // remove '\n' from the end of the message
    
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}	
	return;
}

void recv_post(int socket_desc, char username[])	{
	char buf[1024];
    int buf_len = sizeof(buf);
    int ret, recv_bytes;
    char text[1024];
    char password[30];

	// wait for text of the new post  
	while ( (recv_bytes = recv(socket_desc, text, buf_len, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}
	text[recv_bytes] = '\0';
	

	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for password for the new post  
	while ( (recv_bytes = recv(socket_desc, password, buf_len, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}
	password[recv_bytes] = '\0';

	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    //create the new post
    insert_post(password, text, username);

    //send succesfully message to client
    strcpy(buf, "New post succesfully added to the showcase\0");
    buf_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, buf_len+1, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to the socket");
    }
    return;
}

//send the delete message
void send_delete(int socket_desc)	{
	char buf[1024];
	int msg_len = 0;
	int ret;

	//get id for the post to delete
	printf("Insert the ID of the post you want to delete: ");

    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
        fprintf(stderr, "Error while reading from stdin, exiting...\n");
        exit(EXIT_FAILURE);
    }

    //send id for post to delete
    msg_len = strlen(buf);
    buf[--msg_len] = '\0'; // remove '\n' from the end of the message

    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}

	//get password for post to delete
    printf("Insert the password of post: ");

    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
        fprintf(stderr, "Error while reading from stdin, exiting...\n");
        exit(EXIT_FAILURE);
    }
    
    //send password for post to delete
    msg_len = strlen(buf);
    buf[--msg_len] = '\0'; // remove '\n' from the end of the message

    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}
	return;
}

//receive the delete message from client
void recv_delete(int socket_desc, char username[])	{
	char buf[1024];
	char password[30];
    int buf_len = sizeof(buf);
    int ret, recv_bytes;
	int id;

	// wait for id of the post to delete
	while ( (recv_bytes = recv(socket_desc, buf, buf_len, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}
	buf[recv_bytes] = '\0';
	id = atoi(buf);

	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for password for the post to delete
	while ( (recv_bytes = recv(socket_desc, password, buf_len, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}
	password[recv_bytes] = '\0';

	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    //delete the post
    delete_post(id, password, username);

    //send succesfully deleted message
    strcpy(buf, "Post succesfully deleted from showcase\0");
    buf_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, buf_len+1, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to the socket");
    }
    return;
}











