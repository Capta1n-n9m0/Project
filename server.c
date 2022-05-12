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
    print_haiku(select_random(w));
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

void sigquit_v3(int sig){
    haiku h;
    if(read_haiku(western, &h) == -1){
        if(errno != ENOMSG) error("read_haiku");
        raise(SIGUSR1);
    } else print_haiku(h);
}

void sigusr1_v3(int sig){
    haiku h;
    for (int i = 0; i < 3; i++) {
        h = select_random(w);
        if (write_haiku(western, &h) == -1) error("write_haiku");
    }
    raise(SIGQUIT);
}

void sigint_v3(int sig){
    haiku h;
    if(read_haiku(japanese, &h) == -1){
        if(errno != ENOMSG) error("read_haiku");
        raise(SIGUSR2);
    } else print_haiku(h);

}

void sigusr2_v3(int sig){
    haiku h;
    for (int i = 0; i < 3; i++) {
        h = select_random(j);
        if (write_haiku(japanese, &h) == -1) error("write_haiku");
    }
    raise(SIGINT);
}

void server_v3(){
    puts("[SERVER] Server V3 is up!");
    init_books();
    int id = create_queue();
    signal(SIGINT, sigint_v3);
    signal(SIGQUIT, sigquit_v3);
    signal(SIGUSR1, sigusr1_v3);
    signal(SIGUSR2, sigusr2_v3);
    signal(SIGCHLD, sigchld_v1);
    while (run_condition){
        pause();
    }
    printf("[SERVER] Server is terminating.\n");
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


