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
	char buf[4096];
    int msg_len;
    int ret, recv_bytes;

	if (!empty_showcase())	{
		semaphore_wait();
		showcase* punt = bacheca;
		while (punt != NULL) {
			//write post into buffer
			sprintf(buf, "Time: %sAuthor: %s\nObject: %s\nText: %s\nID: %d\n", 
				ctime(&(punt->element.date)), punt->element.author, punt->element.object, punt->element.text, punt->element.id);
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
	    semaphore_post();
	}else{
		//the showcase is empty....
		sprintf(buf, "Hi! Unfortunately there are no posts on the showcase.\n");
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
	}
	
	//send finished command
	sprintf(buf,"finished");
	msg_len = strlen(buf);
	while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
		if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to the socket");
	}

	return;
}

//receive the showcase from the server
void recv_showcase(int socket_desc)	{
	char buf[4096];
	int msg_len;
    int buf_len = sizeof(buf);
    int ret;

	printf("**************************************************************************************\n");
	printf("--------------------------------------------------------------------------------------\n");
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
	    printf("--------------------------------------------------------------------------------------\n");
	}
	printf("**************************************************************************************\n");
	return;
}

//send new post
void send_post(int socket_desc)	{
	char buf[4096];
	int msg_len = 0;
	int ret;

	while (1)	{
		//get object for new post
		printf("Insert object of the post (max 50 characters): ");

	    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
	        fprintf(stderr, "Error while reading from stdin, exiting...\n");
	        exit(EXIT_FAILURE);
	    }

	    //send object for new post
	    msg_len = strlen(buf);
	    buf[--msg_len] = '\0'; // remove '\n' from the end of the message
	    if (strlen(buf) > 0 && strlen(buf) < 50)	{
	    	break;
	    }
	    printf("Error: too many characters!\n");
	}
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}

	while (1)	{
		//get text for new post
		printf("Insert text of the post (max 1900 characters): ");

	    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
	        fprintf(stderr, "Error while reading from stdin, exiting...\n");
	        exit(EXIT_FAILURE);
	    }

	    //send text for new post
	    msg_len = strlen(buf);
	    buf[--msg_len] = '\0'; // remove '\n' from the end of the message
	    if (strlen(buf) > 0 && strlen(buf) < 1900)	{
	    	break;
	    }
	    printf("Error: too many characters!\n");
	}
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}

	while (1)	{
		//get password for new post
	    printf("Insert password for post (max 15 characters): ");

	    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
	        fprintf(stderr, "Error while reading from stdin, exiting...\n");
	        exit(EXIT_FAILURE);
	    }

	    //send text for new post
	    msg_len = strlen(buf);
	    buf[--msg_len] = '\0'; // remove '\n' from the end of the message
	    if (strlen(buf) > 0 && strlen(buf) < 15)	{
	    	break;
	    }
	    printf("Error: too many characters!\n");
	}
    
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

void recv_post(int socket_desc, char* username)	{
	char buf[4096];
    int buf_len = sizeof(buf);
    int ret, recv_bytes;
    char object[100];
    char text[3800];
    char password[30];

	// wait for object of the new post  
	while ( (recv_bytes = recv(socket_desc, object, buf_len, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	    ERROR_HELPER(-1, "Cannot read from socket");
	}
	object[recv_bytes] = '\0';
	
	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to socket");
    }

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
    if (insert_post(username, object, text, password))	{
    	//send succesfully message to client
    	strcpy(buf, "NEW POST SUCCESFULLY ADDED TO THE SHOWCASE.\0");
    }else{
    	//send unsuccesfully message to client
    	strcpy(buf, "NEW POST HAS NOT BEEN ADDED TO THE SHOWCASE\0");
    }

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
    
	//send deleted message
	strcpy(buf, delete_post(id, password, username));

    buf_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, buf_len+1, 0)) < 0 ) {
        if (errno == EINTR) continue;
        ERROR_HELPER(-1, "Cannot write to the socket");
    }
    return;
}











