#include "socket_util.h"

//check if error from socket
int sock_error(int sock_bytes)	{
	if (sock_bytes <= 0) return 1;
	return 0;
}


//send the showcase in server
int send_showcase(int socket_desc)	{
	char buf[4096];
    int msg_len;
    int ret, recv_bytes, err = 0;

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
        		if (errno == EAGAIN) break;
		    }
    		if (sock_error(ret)) {
    			err = 1;
    			break;
    		}

	        // wait for client's ACK  
	        while ( (recv_bytes = recv(socket_desc, buf, 3, 0)) < 0 ) {
	            if (errno == EINTR) continue;
        		if (errno == EAGAIN) break;
	        }
    		if (sock_error(recv_bytes)) {
    			err = 1;
    			break;
    		} 
        	punt = punt->next;
	    }
	    semaphore_post();
		if (err) return 0;
	}else{
		//the showcase is empty....
		sprintf(buf, "Unfortunately there are no posts on the showcase.\n");
		msg_len = strlen(buf);
	    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
	        if (errno == EINTR) continue;
        	if (errno == EAGAIN) break;
	    }
    	if (sock_error(ret)) return 0;

	    // wait for client's ACK  
	    while ( (recv_bytes = recv(socket_desc, buf, 3, 0)) < 0 ) {
	        if (errno == EINTR) continue;
        	if (errno == EAGAIN) break;
	    } 
    	if (sock_error(recv_bytes)) return 0;
	}

	//send finished command
	sprintf(buf,"finished");
	msg_len = strlen(buf);
	while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0 ) {
		if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
	}
    if (sock_error(ret)) return 0;

	return 1;
}

//receive the showcase in client
int recv_showcase(int socket_desc)	{
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
        }
    	if (sock_error(msg_len)) return 0;

        //check if the showcase is finished
        if (msg_len == 8 && !memcmp(buf, "finished", 8)) 	{
        	break;
        }

        //send ACK to server to confirm
		while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
            if (errno == EINTR) continue;
        }
    	if (sock_error(ret)) return 0;
	    buf[msg_len] = '\0';

	    printf("%s", buf);
	    printf("--------------------------------------------------------------------------------------\n");
	}
	printf("**************************************************************************************\n");
	return 1;
}

//send new post in client
int send_post(int socket_desc)	{
	char buf[4096];
	int msg_len = 0;
	int ret;

	while (1)	{
		//get object for new post
		printf("Insert object of the post (max 50 characters): ");

	    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
	        fprintf(stderr, "Error while reading from stdin, exiting...\n");
	        return 0;
	    }

	    //send object for new post
	    msg_len = strlen(buf);
	    buf[--msg_len] = '\0'; // remove '\n' from the end of the message
	    if (strlen(buf) > 0 && strlen(buf) <= 50)	{
	    	break;
	    }
	    printf("Error: too many characters!\n");
	}
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
    }
    if (sock_error(ret)) return 0;

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	}
    if (sock_error(msg_len)) return 0;

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
	    if (strlen(buf) > 0 && strlen(buf) <= 1900)	{
	    	break;
	    }
	    printf("Error: too many characters!\n");
	}
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
    }
    if (sock_error(ret)) return 0;

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	}
    if (sock_error(msg_len)) return 0;

	while (1)	{
		//get password for new post
	    printf("Insert password for post (max 15 characters): ");

	    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
	        fprintf(stderr, "Error while reading from stdin, exiting...\n");
	        return 0;
	    }

	    //send text for new post
	    msg_len = strlen(buf);
	    buf[--msg_len] = '\0'; // remove '\n' from the end of the message
	    if (strlen(buf) > 0 && strlen(buf) <= 15)	{
	    	break;
	    }
	    printf("Error: too many characters!\n");
	}
    
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
    }
    if (sock_error(ret)) return 0;

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	}
    if (sock_error(ret)) return 0;	
	return 1;
}

// receive new post in server
int recv_post(int socket_desc, char* username)	{
	char buf[4096];
    int buf_len = sizeof(buf);
    int ret, recv_bytes;
    char object[100];
    char text[3800];
    char password[30];

	// wait for object of the new post  
	while ( (recv_bytes = recv(socket_desc, object, sizeof(object), 0)) < 0 ) {
	    if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
	}
	if (sock_error(recv_bytes)) return 0;
	object[recv_bytes] = '\0';
	
	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
    }
	if (sock_error(ret)) return 0;

    // wait for text of the new post  
	while ( (recv_bytes = recv(socket_desc, text, sizeof(text), 0)) < 0 ) {
	    if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
	}
	if (sock_error(recv_bytes)) return 0;
	text[recv_bytes] = '\0';
	
	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
    }
	if (sock_error(ret)) return 0;

    // wait for password for the new post  
	while ( (recv_bytes = recv(socket_desc, password, sizeof(password), 0)) < 0 ) {
	    if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
	}
	if (sock_error(recv_bytes)) return 0;
	password[recv_bytes] = '\0';

	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
    }
	if (sock_error(ret)) return 0;

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
        if (errno == EAGAIN) break;
    }
	if (sock_error(ret)) return 0;
    return 1;
}

//send the delete message command in client
int send_delete(int socket_desc)	{
	char buf[1024];
	int msg_len = 0;
	int ret;

	//get id for the post to delete
	int check_id = 1;
	while(check_id)	{
		check_id = 0;
		printf("Insert the ID of the post you want to delete: ");

	    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
	        fprintf(stderr, "Error while reading from stdin, exiting...\n");
	        return 0;
	    }
	    msg_len = strlen(buf);
	    buf[--msg_len] = '\0'; // remove '\n' from the end of the message
	    for (int i=0; i<msg_len; i++)	{
	    	if (!isdigit(buf[i])) {
	    		printf("ERROR: you didn't inserted a number!\n");
	    		check_id = 1;
	    		break;
	    	}
	    }
	}

    //send id for post to delete
    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
    }
    if (sock_error(ret)) return 0;

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	}
    if (sock_error(msg_len)) return 0;

	//get password for post to delete
    printf("Insert the password of post: ");

    if (fgets(buf, sizeof(buf), stdin) != (char*)buf) {
        fprintf(stderr, "Error while reading from stdin, exiting...\n");
        return 0;
    }
    
    //send password for post to delete
    msg_len = strlen(buf);
    buf[--msg_len] = '\0'; // remove '\n' from the end of the message

    while ( (ret = send(socket_desc, buf, msg_len, 0)) < 0) {
    	if (errno == EINTR) continue;
    }
    if (sock_error(ret)) return 0;

    // wait for server's ACK  
	while ( (msg_len = recv(socket_desc, buf, 3, 0)) < 0 ) {
	    if (errno == EINTR) continue;
	}
    if (sock_error(msg_len)) return 0;
	return 1;
}

//receive the delete message in server
int recv_delete(int socket_desc, char username[])	{
	char buf[1024];
	char password[30];
    int buf_len = sizeof(buf);
    int ret, recv_bytes;
	int id;

	// wait for id of the post to delete
	while ( (recv_bytes = recv(socket_desc, buf, buf_len, 0)) < 0 ) {
	    if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
	}
	if (sock_error(recv_bytes)) return 0;
	buf[recv_bytes] = '\0';
	id = atoi(buf);

	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
    }
	if (sock_error(ret)) return 0;

    // wait for password for the post to delete
	while ( (recv_bytes = recv(socket_desc, password, buf_len, 0)) < 0 ) {
	    if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
	}
	if (sock_error(recv_bytes)) return 0;
	password[recv_bytes] = '\0';

	//send ACK to client to confirm
	while ( (ret = send(socket_desc, "ACK", 3, 0)) < 0) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
    }
	if (sock_error(ret)) return 0;
    
	//send deleted message
	strcpy(buf, delete_post(id, password, username));

    buf_len = strlen(buf);
    while ( (ret = send(socket_desc, buf, buf_len+1, 0)) < 0 ) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN) break;
    }
	if (sock_error(ret)) return 0;
    return 1;
}











