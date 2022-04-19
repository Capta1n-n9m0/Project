#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "server.h"
#include "haiku.h"
#include "queue.h"
#include <sys/shm.h>

book w, j;

#ifdef STANDALONE
#define s_main main
#endif

void error_s(const char *msg){
    perror(msg);
    exit(2);
}

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
    printf("[SERVER]Hello from server v2!\n");
    haiku h; category c; init_books();
    for(int i = 0; i < 6; i++){
        printf("[SERVER]#%d.Sending haiku category ", i);
        if(i%2) {
            c = japanese;
            h = select_random(j);
            puts("japanese.");
        } else {
            c = western;
            h = select_random(w);
            puts("western.");
        }
        if(write_haiku(c,&h)==-1) error_s("write_haiku");
    }
    clear_books();
}

int s_main(){
    key_t k = ftok("/dev/null", '1');
    if(k == -1) error_s("ftok");
    int id = shmget(k,  sizeof(pid_t), 0644 | IPC_CREAT);
    if(id == -1) error_s("shmget");

    // writing server process id
    pid_t server = getpid();
    int *s = shmat(id, NULL, 0);
    *s = server;

    server_v1();
    return 0;
}


