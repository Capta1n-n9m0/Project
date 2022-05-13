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

book w = {0}, j = {0};
bool run_condition = true;

#ifdef STANDALONE
#define s_main main
#endif

void init_books(){
    j = read_category(japanese);
    w = read_category(western);
}
void clear_books(){
    if(w.size)free_book(&w);
    if(j.size)free_book(&j);
}

static void error(const char *msg){
    // cleanup function
    perror(msg);
    clear_books();
    int id;
    if((id = access_queue()) != -1) remove_queue(id);
    exit(2);
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


void *writer_thread_function(void *arg){
    printf("[WRITER] Writer thread is up!\n");
    haiku h;
    bool thread_run_condition = true;
    sigset_t set;
    int sig;
    if(sigemptyset(&set) == -1) error("sigemptyset");
    if(sigaddset(&set, SIGINT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGQUIT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGCHLD) == -1) error("sigaddset");
    while (thread_run_condition){
        if (sigwait(&set, &sig) == -1) error("sigwait");
        switch (sig) {
            case SIGINT:
                printf("[WRITER] SIGINT! Refilling japanese queue with 3 haiku!\n");
                for(int i = 0; i < 3; i++){
                    h = select_random(j);
                    if(write_haiku(japanese, &h) == -1) error("write_haiku");
                }
                if(pthread_kill(reader_thread, SIGINT) != 0) error("pthread_kill");
                break;
            case SIGQUIT:
                printf("[WRITER] SIGQUIT! Refilling western queue with 3 haiku!\n");
                for(int i = 0; i < 3; i++){
                    h = select_random(w);
                    if(write_haiku(western, &h) == -1) error("write_haiku");
                }
                if(pthread_kill(reader_thread, SIGQUIT) != 0) error("pthread_kill");
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
    if(sigemptyset(&set) == -1) error("sigemptyset");
    if(sigaddset(&set, SIGINT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGQUIT) == -1) error("sigaddset");
    if(sigaddset(&set, SIGCHLD) == -1) error("sigaddset");
    while (thread_run_condition){
        if (sigwait(&set, &sig) == -1) error("sigwait");
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
    int id;
    if((id = create_queue()) == -1) error("create_queue");
    if(signal(SIGINT, sigint_v3) == SIG_ERR) error("signal");
    if(signal(SIGQUIT, sigquit_v3) == SIG_ERR) error("signal");
    if(signal(SIGCHLD, sigchld_v3) == SIG_ERR) error("signal");
    if(pthread_create(&writer_thread, NULL, writer_thread_function, NULL) != 0) error("pthread_create");
    if(pthread_create(&reader_thread, NULL, reader_thread_function, NULL) != 0) error("pthread_create");
    while (run_condition){
        pause();
    }
    printf("[SERVER] Server is terminating.\n");
    if(pthread_join(writer_thread, NULL) != 0) error("pthread_join");
    if(pthread_join(reader_thread, NULL) != 0) error("pthread_join");
    remove_queue(id);
    clear_books();
}

int s_main(int argc, char **argv){
    if(argc <= 1){
        fprintf(stderr, "Please provide version number, 1 or 3, as first argument: ./server 1\n");
        exit(1);
    }
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


