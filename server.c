#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "server.h"
#include "haiku.h"
#include "queue.h"
#include <sys/shm.h>
#include <pthread.h>

book w, j;
haiku haiku1;
pthread_mutex_t  w_book_lock, j_book_lock, haiku1_lock;

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


void server_v1(){
    init_books();
    printf("Hello from server v1!\n");
    sigset_t sigset1;
    int sig = 0;
    sigemptyset(&sigset1);
    for(int i = 0; i < 100; i++){
        if(sigwait(&sigset1, &sig) != -1) error("sigwait");
        switch (sig) {
            case SIGQUIT:
                print_haiku(select_random(w));
            case SIGINT:
                print_haiku(select_random(j));
            default:
                error("signal");
        }
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

void *writer_thread_function(void *agr){
    printf("[WRITER] Writer thread is up.\n");
    sigset_t set;
    sigemptyset(&set);
    if(sigaddset(&set, SIGUSR1) == -1) error("sigaddset");
    while (1){
        if (sigwait(&set, SIGUSR1) != SIGUSR1)error("sigwait");


    }
}

void sever_v3(){
    if(pthread_mutex_init(&w_book_lock, NULL) != 0) error("pthread_mutex_init");
    if(pthread_mutex_init(&j_book_lock, NULL) != 0) error("pthread_mutex_init");
    if(pthread_mutex_init(&haiku1_lock, NULL) != 0) error("pthread_mutex_init");



    pthread_mutex_destroy(&w_book_lock);
    pthread_mutex_destroy(&j_book_lock);
    pthread_mutex_destroy(&haiku1_lock);
}

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


