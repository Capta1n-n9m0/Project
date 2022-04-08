#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
#include "client.h"



void v1(){
    pid_t proc = fork();
    if(proc > 0){
        // father
        client_v1(proc);
    } else if(proc == 0){
        // son
        server_v1(1)();
    } else { // proc < 0
        // error
        perror("fork");
        exit(2);
    }
}


int main() {
    srand(time(NULL));
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
