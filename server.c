#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"


void sigint1(){
    signal(SIGINT, sigint1);
    printf("Jap\n");
}

void sigquit1(){
    signal(SIGQUIT, sigquit1);
    printf("West\n");
}


void server_v1(){
    printf("Hello from server\n");
    signal(SIGINT, sigint1);
    signal(SIGQUIT, sigquit1);
    for(int i = 0; i < 100; i++) pause();
}


