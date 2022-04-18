#define	_GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"
#include "haiku.h"
#include "queue.h"

void client_v1(pid_t t){
    sleep(1);
    printf("Hello from client v1!\n");
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

void client_v2(){
    sleep(1);
    printf("Hello from client v2!\n");
    haiku h;
    category c;
    for(int i = 0; i < 6; i++){
        printf("#%d. Reading book category ", i);
        if(i%2) {
            c = western;
            puts("western.");
        }
        else {
            c = japanese;
            puts("japanese.");
        }
        if(read_haiku(c, &h) == -1) exit(2);
        print_haiku(h);
    }
}
