#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "server.h"
#include "haiku.h"
#include "queue.h"
#include <sys/shm.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

book w, j;
bool run_condition = true;

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


void sigquit_v1(int sig){
    signal(SIGQUIT, sigquit_v1);
    print_haiku(select_random(w));
}
void sigint_v1(int sig){
    signal(SIGINT, sigquit_v1);
    print_haiku(select_random(j));
}
void sigchld_v1(int sig){
    run_condition = false;
}

void server_v1(){
    init_books();
    printf("[SERVER] Hello from server v1!\n");
    signal(SIGQUIT, sigquit_v1);
    signal(SIGINT, sigint_v1);
    signal(SIGCHLD, sigchld_v1);
    while(run_condition){
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
void v2_haiku_reader() {
    printf("[READER]Hello from reader v2!\n");
    sleep(1);
    haiku h;
    category c;
    for (int i = 0; i < 6; i++) {
        printf("[READER]#%d. Reading book category ", i);
        if (i % 2) {
            c = western;
            puts("western.");
        } else {
            c = japanese;
            puts("japanese.");
        }
        if (read_haiku(c, &h) == -1) error("read_haiku");
        print_haiku(h);
    }
}

pthread_t writer_thread, reader_thread;

void *writer_thread_function(void *arg){
    printf("[WRITER] Writer thread is up!\n");
    haiku h;
    bool thread_run_condition = true;
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGCHLD);
    while (thread_run_condition){
        sigwait(&set, &sig);
        switch (sig) {
            case SIGINT:
                printf("[WRITER] SIGINT! Refilling japanese queue with 3 haiku!\n");
                for(int i = 0; i < 3; i++){
                    h = select_random(j);
                    write_haiku(japanese, &h);
                }
                pthread_kill(reader_thread, SIGINT);
                break;
            case SIGQUIT:
                printf("[WRITER] SIGQUIT! Refilling western queue with 3 haiku!\n");
                for(int i = 0; i < 3; i++){
                    h = select_random(w);
                    write_haiku(western, &h);
                }
                pthread_kill(reader_thread, SIGQUIT);
                break;
            case SIGCHLD:
                printf("[WRITER] SIGCHLD! Quiting!\n");
                thread_run_condition = false;
                break;
        }
    }
    printf("[WRITER] Writer is terminating!\n");
    return NULL;
}

void *reader_thread_function(void *arg){
    printf("[READER] READER thread is up!\n");
    haiku h;
    bool thread_run_condition = true;
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGCHLD);
    while (thread_run_condition){
        sigwait(&set, &sig);
        switch (sig) {
            case SIGINT:
                printf("[READER] SIGINT! Reading japanese haiku!\n");
                if(read_haiku(japanese, &h) == -1){
                    if(errno != ENOMSG) error("read_haiku");
                    pthread_kill(writer_thread, SIGINT);
                } else print_haiku(h);
                break;
            case SIGQUIT:
                printf("[READER] SIGQUIT! Reading western haiku!\n");
                if(read_haiku(western, &h) == -1){
                    if(errno != ENOMSG) error("read_haiku");
                    pthread_kill(writer_thread, SIGQUIT);
                } else print_haiku(h);
                break;
            case SIGCHLD:
                printf("[READER] SIGCHLD! Quiting! \n");
                thread_run_condition = false;
                break;
        }
    }
    printf("[READER] READER is terminating!\n");
    return NULL;

}

void sigquit_v3(int sig){
    pthread_kill(reader_thread, SIGQUIT);
}
void sigint_v3(int sig){
    pthread_kill(reader_thread, SIGINT);
}
void sigchld_v3(int sig){
    pthread_kill(reader_thread, SIGCHLD);
    pthread_kill(writer_thread, SIGCHLD);
    run_condition = false;
}

void server_v3(){
    puts("[SERVER] Server V3 is up!");
    init_books();
    int id = create_queue();
    signal(SIGINT, sigint_v3);
    signal(SIGQUIT, sigquit_v3);
    signal(SIGCHLD, sigchld_v3);
    pthread_create(&writer_thread, NULL, writer_thread_function, NULL);
    pthread_create(&reader_thread, NULL, reader_thread_function, NULL);
    while (run_condition){
        pause();
    }
    printf("[SERVER] Server is terminating.\n");
    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread, NULL);
    remove_queue(id);
    clear_books();
}

int s_main(int argc, char **argv){
    assert(argc > 1);
    key_t k = ftok("/dev/null", '1');
    if(k == -1) error("ftok");
    int id = shmget(k,  sizeof(pid_t), 0644 | IPC_CREAT);
    if(id == -1) error("shmget");

    // writing server process id
    pid_t server = getpid();
    int *s = shmat(id, NULL, 0);
    *s = server;
    printf("[SERVER] Wrote process id(%d) to shared memory(id=%d)\n", server, id);

    switch (argv[1][0]) {
        case '1':
            server_v1();
            break;
        case '3':
            server_v3();
            break;
        default:
            fprintf(stderr, "Not implemented: %s\n", argv[1]);
    }
    return 0;
}


