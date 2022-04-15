#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "server.h"
#include "client.h"
#include "haiku.h"


void v1(){
    pid_t proc = fork();
    printf("fork res: %d\n", proc);
    if(proc > 0){
        // father
        client_v1(proc);
    } else if(proc == 0){
        // son
        server_v1();
    } else { // proc < 0
        // error
        perror("fork");
        exit(2);
    }
}

void *server_v2_wrapper(void *arg){
    server_v2();
    pthread_exit(NULL);
}

void *client_v2_wrapper(void *arg){
    client_v2();
    pthread_exit(NULL);
}

void v2(){
    pthread_t s,c;
    if(pthread_create(&s, NULL, server_v2_wrapper, NULL) != 0) perror("pthread_create");
    if(pthread_create(&c, NULL, client_v2_wrapper, NULL) != 0) perror("pthread_create");
    pthread_join(s,NULL);
    pthread_join(c, NULL);
}

void test(){
    book j = read_book(japanese);
    book w = read_book(western);
    haiku h;
    int choice;
    for(int i = 0; i < 20; i++){
        choice = rand() % 2;
        if(choice) h = select_random(j);
        else h = select_random(w);
        print_haiku(h);
        puts("");
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));
    printf("Haiku project\n");
    assert(argc > 1);
    // test();
    printf("Server version %s\n", argv[1]);
    printf("Client version %s\n", argv[1]);
    switch (argv[1][0]) {
        case '1':
            v1();
            break;
        case '2':
            v2();
            break;
        default:
            puts("Not implemented.");
    }
    getchar();
    return 0;
}
