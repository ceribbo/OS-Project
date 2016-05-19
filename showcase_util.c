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


int empty_showcase()	{
    if(bacheca==NULL) {
    	return 1;
    	printf("BACHECA è VUOTA\n");
    } else return 0;
}

int insert_post(char username[], char object[], char text[], char password[])	{

	//create new element
    showcase* new_element = (showcase*)malloc(sizeof(showcase));
    showcase* punt;
    if (new_element == NULL)	{
    	fprintf(stderr, "malloc() failed.\n");
    	return 0;
    }

    //insert fields
    post post_element;
    strcpy(post_element.author, username);
    strcpy(post_element.object, object);
    strcpy(post_element.text, text);
    strcpy(post_element.password, password);
    post_element.date = time(NULL);

    if(empty_showcase())	{
    	//if showcase is null
    	post_element.id = 0;

    	//assign post to showcase struct
    	new_element->element = post_element;
    	new_element->next = NULL;
        bacheca=new_element;
        return 1;
    } else {
    	//add at the end of list
        punt=bacheca;
        while(punt->next!=NULL)	{
            punt=punt->next;
        }
        post_element.id = (punt->element.id) + 1;
        //assign post to showcase struct
    	new_element->element = post_element;
    	new_element->next = NULL;

        punt->next = new_element;
        return 1;
    }
    return 0;
}

char* delete_post(int id, char password[], char username[])	{

	if (!empty_showcase())	{
		//create new element
	    showcase* punt;
	    showcase* previous;

	    punt=bacheca;
	    while(punt!=NULL)	{
	    	if (punt->element.id == id)	{
	    		if (strlen(punt->element.author) == strlen(username) && !memcmp(punt->element.author, username, strlen(username))) {
	    			if (strlen(punt->element.password) == strlen(password) && !memcmp(punt->element.password, password, strlen(password))) {
		    			if (punt == bacheca)	{
		    				bacheca = punt->next;
	    					return "Post succesfully deleted from showcase.";
		    			}else{
		    				previous->next = punt->next;
		    				free(previous);
	    					return "Post succesfully deleted from showcase.";
		    			}
		    		}else{
		    			return "Error: password was incorrect.";
		    		}
		    	}else{
					return "Error: post was not created by you. You can't delete it.";
		    	}
	    	}
	    	previous = punt;
	        punt = punt->next;     
	    }
	    return "Error: wrong ID, post not found.";
	}
	return "Error: the showcase is empty";
}

void print_showcase()	{	

	printf("**************************************************************************************\n");
	if (!empty_showcase())	{    
		showcase* punt = bacheca;
		while(punt != NULL)	{

			printf("*-------------------------------\n");
			printf("*Time: %s", ctime(&(punt->element.date)));
			printf("*Author: %s\n", punt->element.author);
			printf("*Object: %s\n", punt->element.object);
			printf("*Text: %s\n", punt->element.text);
			printf("*Password: %s\n", punt->element.password);
			printf("*ID: %d\n", punt->element.id);

	        punt=punt->next;

	    }
	}else{
		printf("Showcase is empty.\n");
	}
	printf("**************************************************************************************\n");

	return;
}



