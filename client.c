#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "client.h"

void client_v1(pid_t t){
    printf("Hello from client\n");
    int c;
    for(int i = 0; i < 100; i++){
        c = rand()%2;
        printf("%d\n",c);
        if(c) kill(t, SIGINT);
        else kill(t, SIGQUIT);
    }
}


