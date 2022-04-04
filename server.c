#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"


void sigint(){
    signal(SIGINT, sigint);
    printf("Jap\n");
}

void sigquit(){
    signal(SIGINT, sigquit);
    printf("West\n");
}


void server_v1(){
    printf("Hello from server\n");
    signal(SIGINT, sigint);
    signal(SIGQUIT, sigquit);
    for(int i = 0; i < 100; i++) pause();
}


