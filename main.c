#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
#include "client.h"
#include "haiku.h"


void v1(){
    pid_t proc = fork();
    if(proc > 0){
        // father
        client_v1(proc);
    } else if(proc == 0){
        // son
        server_v1(1);
    } else { // proc < 0
        // error
        perror("fork");
        exit(2);
    }
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

int main() {
    srand(time(NULL));
    test();
//    printf("Haiku project\n");
//    int version = 1;
//    printf("Server version %d\n", version);
//    printf("Client version %d\n", version);
//    switch (version) {
//        case 1:
//            v1();
//            break;
////        case 2:
////            break;
//    }

    return 0;
}
