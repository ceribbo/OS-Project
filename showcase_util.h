#ifndef SHOWCASE_UTIL_H
#define	SHOWCASE_UTIL_H

#include "lib.h"

typedef struct {
    int id;	//4 byte
    char author[30]; //30 byte
    char password[30]; //30 byte
    char object[100]; //100 byte
    char text[3800]; //3800 byte
    time_t date; //8 byte
    //+ 80 byte to transmit on socket
} post;

typedef struct showcase_node{
    post element;
    struct showcase_node* next;
} showcase;

showcase* bacheca;

sem_t* semaphore;


//add new post into the showcase
int insert_post(char author[], char object[], char text[], char password[]);

//delete post from showcase
char* delete_post(int id, char password[], char username[]);

//print the showcase
void print_showcase();

//check if the showcase is empty
int empty_showcase();

//make a wait on the semaphore
void semaphore_wait();

//make a post on the semaphore
void semaphore_post();

#endif