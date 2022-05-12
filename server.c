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
#include <stdbool.h>
#include <errno.h>

book w, j;
pthread_mutex_t  w_book_lock, j_book_lock;

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
    bool run_condition = true;
    printf("[SERVER] Hello from server v1!\n");
    sigset_t set;
    int sig = 0;
    sigemptyset(&set);
    if(sigaddset(&set, SIGQUIT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGINT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGCHLD) == -1) error("sigaddset");
    while(run_condition){
        if(sigwait(&set, &sig) == -1) error("sigwait");
        switch (sig) {
            case SIGQUIT:
                print_haiku(select_random(w));
                break;
            case SIGINT:
                print_haiku(select_random(j));
                break;
            case SIGCHLD:
                printf("[SERVER] Server terminating\n");
                run_condition = false;
                break;
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

void *writer_thread_function(void *arg){
    printf("[WRITER] Writer thread is up.\n");
    pthread_t main_thread = ((pthread_t *)arg)[0];
    bool run_condition = true;
    sigset_t set;
    haiku h;
    int sig;
    sigemptyset(&set);
    if(sigaddset(&set, SIGUSR1) == -1) error("sigaddset");
    if(sigaddset(&set, SIGUSR2) == -1) error("sigaddset");
    if(sigaddset(&set, SIGCHLD) == -1) error("sigaddset");
    while (run_condition){
        if (sigwait(&set, &sig) == -1) {
            perror("sigwait");
            break;
        }
        switch (sig) {
            case SIGUSR1:
                printf("[WRITER] writing 3 japanese haiku to the queue\n");
                pthread_mutex_lock(&j_book_lock);

                // write 3 japanese haiku
                for(int i = 0; i < 3; i++){
                    h = select_random(j);
                    if(write_haiku(japanese, &h) == -1) error("write_haiku");
                }

                pthread_mutex_unlock(&j_book_lock);
                pthread_kill(main_thread, SIGUSR1);
                break;
            case SIGUSR2:
                printf("[WRITER] writing 3 western haiku to the queue\n");
                pthread_mutex_lock(&w_book_lock);

                // write 3 western haiku
                for(int i = 0; i < 3; i++){
                    h = select_random(w);
                    if(write_haiku(western, &h) == -1) error("write_haiku");
                }

                pthread_mutex_unlock(&w_book_lock);
                pthread_kill(main_thread, SIGUSR1);
                break;
            case SIGCHLD:
                printf("[WRITER] Writer is terminating.");
                run_condition = false;
                break;
            default:
                error("Wrong value");
        }
    }
    return NULL;
}

void sever_v3(){
    init_books();
    haiku h;
    sigset_t set;
    int sig;
    bool run_condition = true;
    category last_category;
    pthread_t writer_thread, main_thread;
    main_thread = pthread_self();
    sigemptyset(&set);
    if(sigaddset(&set, SIGQUIT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGINT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGCHLD) == -1) error("sigaddset");
    if(sigaddset(&set, SIGUSR1) == -1) error("sigaddset");
    if(pthread_mutex_init(&w_book_lock, NULL) != 0) error("pthread_mutex_init");
    if(pthread_mutex_init(&j_book_lock, NULL) != 0) error("pthread_mutex_init");
    if(pthread_create(&writer_thread, NULL, writer_thread_function, &main_thread) != 0) perror("pthread_create");

    while (run_condition){
        if(sigwait(&set, &sig) == -1) error("sigwait");
        switch (sig) {
            case SIGQUIT:
                last_category = western;
                break;
            case SIGINT:
                last_category = japanese;
                break;
            case SIGCHLD:
                printf("[SERVER] Server terminating\n");
                pthread_kill(writer_thread, SIGCHLD);
                run_condition = false;
                break;
            case SIGUSR1:
                break;
            default:
                error("signal");
        }
        if(read_haiku(last_category, &h) != -1){
            if(errno != ENOMSG)error("read_haiku");
            switch (last_category) {
                case japanese:
                    pthread_kill(writer_thread, SIGQUIT);
                    break;
                case western:
                    pthread_kill(writer_thread, SIGINT);
                    break;
                default:
                    error("Unknown category");
            }
        } else print_haiku(h);

    }

    pthread_join(writer_thread, NULL);
    pthread_mutex_destroy(&w_book_lock);
    pthread_mutex_destroy(&j_book_lock);
    clear_books();
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


