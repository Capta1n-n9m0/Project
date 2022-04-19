#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

//matrix that associates category of haiku with a signal like a hash table
int category_signal[] = {
        SIGINT, // => category.japanese = 0
        SIGQUIT,// => category.western = 1
};

//function that prints error and quits. it saves few line of code here and there
void error(const char *msg){
    perror(msg);
    exit(2);
}

// to let processes communicate via signals process id of a server is read from shared memory
int main(){
    printf("Hello from client v1!\n");
    srand(time(NULL));

    //access shared memory
    key_t k = ftok("/dev/null", '1');
    if(k == -1) error("ftok");
    int id;
    //waiting for server to write data
    for(int i = 0; i < 5; i++){
        id = shmget(k,  sizeof(pid_t), 0644);
        if(id != -1) break;
        else{
            fprintf(stderr, "Failed accessing shared memory. Server may not opened it yet. Attempt %d out of 5. Trying again in 2 seconds...\n", i+1);
            sleep(2);
        }
    }
    id = shmget(k,  sizeof(pid_t), 0644);
    if(id == -1) error("shmget");

    // reading server process id
    pid_t server;
    int *s = shmat(id, NULL, 0);
    server = *s;
    printf("Server process id: %d\n", server);

    //sending signals to a server
    int sig;
    for(int i = 0; i < 100; i++){
        sig = category_signal[rand()%(sizeof(category_signal)/sizeof(int))];
        printf("[CLIENT] #%d: Sending signal %d to %d\n", i, sig, server);
        kill(server, sig);
        usleep(1000*10);
    }

    // closing shared memory
    shmdt(s);
    shmctl(id, IPC_RMID, 0);
}