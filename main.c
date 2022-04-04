#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
#include "client.h"

#define SERVER(V) server_v##V()
#define CLIENT(V) client_v##V()


void v1(){
    pid_t proc = fork();
    if(proc > 0){
        // father
        CLIENT(1);
    } else if(proc == 0){
        // son
        SERVER(1);
    } else { // proc < 0
        // error
        perror("fork");
        exit(2);
    }
}


int main() {
    printf("Haiku project\n");
    int version = 1;
    printf("Server version %d\n", version);
    printf("Client version %d\n", version);
    switch (version) {
        case 1:
            v1();
            break;
//        case 2:
//            break;
    }

    return 0;
}
