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
     if(bacheca==NULL) return 1;
     else return 0;
}

void insert_post(char* author, char* pass, char* testo)	{

	//create new element
    showcase* new_element = (showcase*)malloc(sizeof(showcase));
    showcase* punt;
    if (new_element == NULL)	{
    	fprintf(stderr, "malloc() failed.\n");
    }

    //insert fields
    post post_element;
    post_element.author = author;
   	post_element.password = pass;
    post_element.text = testo;
    post_element.date = time(NULL);

	printf("TESTO= %s, PASSWORD= %s\n",post_element.text, post_element.password);

    if(empty_showcase())	{
    	//if showcase is null
    	post_element.id = 0;

    	//assign post to showcase struct
    	new_element->element = post_element;
    	new_element->next = NULL;
        bacheca=new_element;
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
    }
    return;
}

void delete_post(int id, char* password, char* username)	{

	if (!empty_showcase())	{
		//create new element
	    showcase* punt;
	    showcase* previous;

	    punt=bacheca;
	    while(punt!=NULL)	{
	    	if (punt->element.id == id)	{
	    		if (punt->element.author==username)	{
		    		if (punt->element.password==password)	{
		    			if (punt == bacheca)	{
		    				bacheca = punt->next;
		    				break;
		    			}else{
		    				previous->next = punt->next;
		    				free(previous);
		    				break;
		    			}
		    		}
		    	}
	    	}
	    	previous = punt;
	        punt = punt->next;        
	    }
	    free(punt);
	}
	return;
}

void print_showcase()	{	

	if (!empty_showcase())	{    
		showcase* punt = bacheca;
		printf("**************************************************************************************\n");
		while(punt != NULL)	{

			printf("*-------------------------------\n");
			printf("*Time: %s", ctime(&(punt->element.date)));
			printf("*Author: %s\n", punt->element.author);
			printf("*Text: %s\n", punt->element.text);
			printf("*Password: %s\n", punt->element.password);
			printf("*ID: %d\n", punt->element.id);

	        punt=punt->next;

	    }
		printf("**************************************************************************************\n");
	}else{
		printf("Showcase is empty.\n");
	}

	return;
}



