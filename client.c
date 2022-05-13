#define	_GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"
#include "haiku.h"
#include "queue.h"
#include <sys/shm.h>
#include <stdbool.h>

// clever macro that if defined lets running t_main as main function of file
// if macro is not defined, prevents conflicts of "main"s
#ifdef STANDALONE
#define c_main main
#endif

static void error(const char *msg){
    perror(msg);
    exit(2);
}

void client_v1(pid_t t){
    sleep(1);
    printf("[CLIENT]Hello from client v1!\n");
    int c;
    for(int i = 0; i < 100; i++){
        usleep(10*1000);
        c = rand()%2;
        if(c) {
            printf("[CLIENT]#%d. Sending SIGINT to %d(%d)\n", i, t, c);
            kill(t, SIGINT);
        }
        else {
            printf("[CLIENT]#%d. Sending SIGQUIT to %d(%d)\n", i, t, c);
            kill(t, SIGQUIT);
        }
    }
    kill(t, SIGCHLD);
}

void client_v3(pid_t t){
    sleep(1);
    printf("[CLIENT] Hello from client v3!\n");
    bool run_condition = true;
    while (run_condition){
        puts("Press 1-3 to perform action");
        printf("1 : Print japanese haiku.\n");
        printf("2 : Print western haiku.\n");
        printf("3 : Quit.\n");
        char c;
        switch (c = getchar()) {
            case '1':
                printf("[CLIENT] Sending SIGINT to %d\n", t);
                kill(t, SIGINT);
                break;
            case '2':
                printf("[CLIENT] Sending SIGQUIT to %d\n", t);
                kill(t, SIGQUIT);
                break;
            case '3':
                printf("[CLIENT] Sending SIGCHLD to %d\n", t);
                kill(t, SIGCHLD);
                run_condition = false;
                printf("[CLIENT] Client is terminating\n");
                break;
            default:
                printf("\nWRONG KEY : %c(%d)\"\"\n\n", c, c);
                break;
        }
        getchar();
    }

}

int c_main(int argc, char **argv){
    if(argc <= 1){
        fprintf(stderr, "Please provide version number, 1 or 3, as first argument: ./client 1\n");
        exit(1);
    }
    //access shared memory
    key_t k = ftok("/dev/null", '1');
    if(k == -1) error("ftok");
    int id;
    //waiting for server to write data
    for(int i = 0; i < 5; i++){
        id = shmget(k,  sizeof(pid_t), 0644);
        if(id != -1) break;
        else{
            fprintf(stderr, "[CLIENT]Failed accessing shared memory. Server may not opened it yet. Attempt %d out of 5. Trying again in 2 seconds...\n", i+1);
            sleep(2);
        }
    }
    id = shmget(k,  sizeof(pid_t), 0644);
    if(id == -1) error("shmget");

    // reading server process id
    pid_t server;
    int *s = shmat(id, NULL, 0);
    server = *s;
    printf("[CLIENT]Server process id: %d\n", server);

    switch (argv[1][0]) {
        case '1':
            client_v1(server);
            break;
        case '3':
            client_v3(server);
            break;
        default:
            fprintf(stderr, "Not implemented: %s\n", argv[1]);
    }

    // closing shared memory
    shmdt(s);
    shmctl(id, IPC_RMID, 0);
    return 0;
}

