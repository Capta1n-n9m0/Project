#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"
#include "haiku.h"

book w, j;

void init_books(){
    j = read_book(japanese);
    w = read_book(western);
}

void sigint1(){
    signal(SIGINT, sigint1);
    printf("Jap\n");
    print_haiku(select_random(j));
}

void sigquit1(){
    signal(SIGQUIT, sigquit1);
    printf("West\n");
    print_haiku(select_random(w));
}


void server_v1(){
    init_books();
    printf("Hello from server\n");
    signal(SIGINT, sigint1);
    signal(SIGQUIT, sigquit1);
    for(int i = 0; i < 100; i++) {
        printf("#%d. Waiting for signal.\n", i);
        pause();
    }
}


