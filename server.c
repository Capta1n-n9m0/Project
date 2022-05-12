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

static void error(const char *msg){
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

void v2_haiku_writer(){
    printf("[WRITER]Hello from writer v2!\n");
    haiku h; category c; init_books();
    for(int i = 0; i < 6; i++){
        printf("[WRITER]#%d.Sending haiku category ", i);
        if(i%2) {
            c = japanese;
            h = select_random(j);
            puts("japanese.");
        } else {
            c = western;
            h = select_random(w);
            puts("western.");
        }
        if(write_haiku(c,&h)==-1) error("write_haiku");
    }
    clear_books();
}
void v2_haiku_reader(){
    printf("[READER]Hello from reader v2!\n");
    sleep(1); haiku h; category c;
    for(int i = 0; i < 6; i++){
        printf("[READER]#%d. Reading book category ", i);
        if(i%2) {
            c = western;
            puts("western.");
        }
        else {
            c = japanese;
            puts("japanese.");
        }
        if(read_haiku(c, &h) == -1) error("read_haiku");
        print_haiku(h);
    }
}
// server should be writer, but client is reader

int s_main(){
    key_t k = ftok("/dev/null", '1');
    if(k == -1) error("ftok");
    int id = shmget(k,  sizeof(pid_t), 0644 | IPC_CREAT);
    if(id == -1) error("shmget");

    // writing server process id
    pid_t server = getpid();
    int *s = shmat(id, NULL, 0);
    *s = server;

    server_v1();
    return 0;
}


