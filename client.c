#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"

void client_v1(pid_t t){
    sleep(1);
    printf("Hello from client\n");
    int c;
    for(int i = 0; i < 100; i++){
        usleep(10*1000);
        c = rand()%2;
        if(c) {
            printf("#%d. Sending SIGINT to %d(%d)\n", i, t, c);
            kill(t, SIGINT);
        }
        else {
            printf("#%d. Sending SIGQUIT to %d(%d)\n", i, t, c);
            kill(t, SIGQUIT);
        }
    }
}


