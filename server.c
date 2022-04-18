#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "server.h"
#include "haiku.h"
#include "queue.h"

book w, j;

void init_books(){
    j = read_book(japanese);
    w = read_book(western);
}
void clear_books(){
    free_book(&w);
    free_book(&j);
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
    printf("Hello from server v1!\n");
    signal(SIGINT, sigint1);
    signal(SIGQUIT, sigquit1);
    for(int i = 0; i < 100; i++) {
        printf("#%d. Waiting for signal.\n", i);
        pause();
    }
    clear_books();
}
// server should be writer, but client is reader
void server_v2(){
    printf("Hello from server v2!\n");
    haiku h;
    category c;
    init_books();
    for(int i = 0; i < 6; i++){
        printf("#%d. Sending book category ", i);
        if(i%2) {
            c = japanese;
            h = select_random(j);
            puts("japanese.");
        }
        else {
            c = western;
            h = select_random(w);
            puts("western.");
        }
        if (write_haiku(c, &h) == -1) exit(2);
    }
    clear_books();
}
