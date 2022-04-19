#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "haiku.h"

// books need to be accessible from signal_handler function, so they are made global
book w, j;

// matrix that associates category of haiku with a signal like a hash table
int category_signal[] = {
    SIGINT, // => category.japanese = 0
    SIGQUIT,// => category.western = 1
};

// function that prints error and quits. it saves few line of code here and there
void error(const char *msg){
    perror(msg);
    exit(2);
}

// looks for corresponding signal in matrix and then prints corresponding haiku
void signal_handler(int signal){
    int i;
    for(i = 0; i < sizeof(category_signal)/sizeof(int); i++) if(category_signal[i] == signal) break;
    haiku h;
    switch ((category)i) {
        case japanese:
            h = select_random(j);
            break;
        case western:
            h = select_random(w);
            break;
        default:
            fprintf(stderr, "Invalid category %d\n", i);
            exit(2);
    }
    print_haiku(h);
}

// to let processes communicate via signals process id of a server is written to shared memory
int main(){
    printf("Hello from client v1!\n");

    //access shared memory
    key_t k = ftok("/dev/null", '1');
    if(k == -1) error("ftok");
    int id = shmget(k,  sizeof(pid_t), 0644 | IPC_CREAT);
    if(id == -1) error("shmget");

    // writing server process id
    pid_t server = getpid();
    int *s = shmat(id, NULL, 0);
    *s = server;

    // creating books of haiku
    j = read_book(japanese);
    w = read_book(western);

    //setting up signals
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs,SIGINT);
    sigaddset(&sigs, SIGQUIT);
    for(int i = 0; i < 100; i++){
        int sig = sigtimedwait(&sigs, NULL, &((struct timespec){2,0}));
        if(sig == -1) fprintf(stderr, "[SERVER] #%d: Signal was not received in 2 seconds, moving on.\n", i);
        printf("Received signal %d\n", sig);
        signal_handler(sig);
    }

    //freeing books
    free_book(&j);
    free_book(&w);
}