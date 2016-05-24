#include "lib.h"

int server_socket_desc, ret;

void exit_server()  {
    printf("\nEXITING FROM SERVER...\n");

    // close server socket
    ret = close(server_socket_desc);
    ERROR_HELPER(ret, "Cannot close socket while server shutting down");

    //free the showcase memory allocated
    free(bacheca);

    if (sem_close(semaphore) == -1) {
        printf("Error closing the semaphore\n");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEM_NAME) == -1) {
        printf("Error unlinking the semaphore\n");
        exit(EXIT_FAILURE);
    }

    // this should never be executed
    exit(EXIT_SUCCESS); 
}


int main(int argc, char* argv[]) {

    //set signals to receive
    signal(SIGINT, exit_server);
    signal(SIGTERM, exit_server);
    signal(SIGQUIT, exit_server);
    signal(SIGILL, exit_server);
    signal(SIGHUP, exit_server);
    signal(SIGSEGV, exit_server);
    signal(SIGPIPE, SIG_IGN);


    // Initialize the semaphore 
    semaphore = (sem_t*) malloc(sizeof (sem_t));
    semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    if (semaphore == SEM_FAILED && errno == EEXIST) {
        sem_unlink(SEM_NAME);
        printf("sem_open failed, open it again!\n");
        semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    }
    if (semaphore == SEM_FAILED) {
        ERROR_HELPER(-1, "Could not create a semaphore\n");
    }

    //test***********************************
    insert_post("edu", "oggettoEDU", "testoEDU", "password"); 
    insert_post("ceribbo", "oggettoCE", "testoCE", "password");
    //fineTest*******************************

    //initialize socket server socket and structures
    int client_desc;
    struct sockaddr_in server_addr = {0};
    int sockaddr_len = sizeof(struct sockaddr_in);

    // initialize socket for listening
    server_socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    ERROR_HELPER(server_socket_desc, "Could not create socket");

    // set server_addr struct
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(SERVER_PORT); 

    // set sockopt
    int reuseaddr_opt = 1;
    ret = setsockopt(server_socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt));
    ERROR_HELPER(ret, "Cannot set SO_REUSEADDR option");

    // bind address to socket
    ret = bind(server_socket_desc, (struct sockaddr*) &server_addr, sockaddr_len);
    ERROR_HELPER(ret, "Cannot bind address to socket");

    // start listening
    ret = listen(server_socket_desc, MAX_CONN_QUEUE);
    ERROR_HELPER(ret, "Cannot listen on socket");

    struct sockaddr_in* client_addr = calloc(1, sizeof(struct sockaddr_in));

    // loop to manage incoming connections spawning handler threads
    while (1) {
        // accept incoming connection
        client_desc = accept(server_socket_desc, (struct sockaddr*) client_addr, (socklen_t*) &sockaddr_len);
        if (client_desc == -1 && errno == EINTR) continue; 
        if (client_desc == -1) {
            printf("Cannot open socket for incoming connection\n");
            continue;
        }

        //allocate thread's information
        pthread_t thread;
        handler_args_t* thread_args = malloc(sizeof(handler_args_t));
        thread_args->socket_desc = client_desc;
        thread_args->client_addr = client_addr;
        
        if (pthread_create(&thread, NULL, client_handler, (void*)thread_args) != 0) {
            fprintf(stderr, "Can't create a new thread, error %d\n", errno);
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread);
        client_addr = calloc(1, sizeof(struct sockaddr_in));
    }
    exit_server();
}
