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
	int var;
	printf("qui ci arribo\n");
	semaphore_wait();
    if(bacheca==NULL) {printf("qui ci arrivo 2"); var = 1;}
    else var = 0;
	semaphore_post();
	return var;
}

int insert_post(char username[], char object[], char text[], char password[])	{
	int ret = 0;
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

	
	semaphore_wait();
    printf("arrivo\n");
    if(empty_showcase())	{
    	//if showcase is null
    	post_element.id = 0;

    	//assign post to showcase struct
    	new_element->element = post_element;
    	new_element->next = NULL;
        bacheca=new_element;
        printf("POST = %d - ADDED BY %s\n", post_element.id, username);
        ret = 1;
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
        printf("POST = %d - ADDED BY %s\n", post_element.id, username);
        ret = 1;
    }
	semaphore_post();
    return ret;
}

char* delete_post(int id, char password[], char username[])	{

	if (!empty_showcase())	{
		
		//create new element
	    showcase* punt;
	    showcase* previous;

		semaphore_wait();
	    punt=bacheca;
	    while(punt!=NULL)	{
	    	if (punt->element.id == id)	{
	    		if (strlen(punt->element.author) == strlen(username) && !memcmp(punt->element.author, username, strlen(username))) {
	    			if (strlen(punt->element.password) == strlen(password) && !memcmp(punt->element.password, password, strlen(password))) {
		    			if (punt == bacheca)	{
		    				bacheca = punt->next;
		    				printf("POST = %d - DELETED BY %s\n", id, username);
							semaphore_post();
	    					return "POST SUCCESFULLY DELETED FROM SHOWCASE";
		    			}else{
		    				previous->next = punt->next;
		    				free(punt);
		    				printf("POST = %d - DELETED BY %s\n", id, username);
							semaphore_post();
	    					return "POST SUCCESFULLY DELETED FROM SHOWCASE";
		    			}
		    		}else{
						semaphore_post();
		    			return "PASSWORD WAS NOT CORRECT";
		    		}
		    	}else{
					semaphore_post();
					return "POST WAS NOT CREATED BY YOU. YOU CAN'T DELETE IT";
		    	}
	    	}
	    	previous = punt;
	        punt = punt->next;     
	    }
		semaphore_post();
	    return "WRONG ID, POST NOT FOUND";
	}
	semaphore_post();
	return "THE SHOWCASE IS EMPTY";
}

void print_showcase()	{	

	printf("**************************************************************************************\n");
	if (!empty_showcase())	{

		semaphore_wait();
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
		semaphore_post();
	}else{
		printf("Showcase is empty.\n");
	}
	printf("**************************************************************************************\n");

	return;
}

//call sem_wait
void semaphore_wait()	{
	int ret;
	if ((ret = sem_wait(semaphore)))	{
	    printf("Can't make sem_post\n");
	    exit(1);
	}
    printf("ARRIVOOOOOO\n");
}

//call sem_post
void semaphore_post()	{
	int ret;
	if ((ret = sem_post(semaphore)))	{
	    printf("Can't make sem_post\n");
	    exit(1);
	}
}



