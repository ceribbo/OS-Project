#ifndef SHOWCASE_UTIL_H
#define	SHOWCASE_UTIL_H

#include "lib.h"

typedef struct {
    int id;
    char* author;
    char* password;
    char* text;
    time_t date;
} post;

typedef struct showcase_node{
    post element;
    struct showcase_node* next;
} showcase;

showcase* bacheca;


//add new post into the showcase
void insert_post(char* author, char* password, char* text);

//delete post from showcase
void delete_post(int id, char* password, char* username);

//print the showcase
void print_showcase();

//check if the showcase is empty
int empty_showcase();

#endif