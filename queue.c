#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <assert.h>
#include "queue.h"

int is_queue = 0;

int create_queue(){
    key_t k;
    int id;
    k = ftok("/dev/null", '2');
    if(k == -1) perror("ftok");
    id = msgget(k, IPC_CREAT | 0666);
    if(id == -1) perror("msgget");
    is_queue = 1;
    return id;
}
int access_queue(){
    key_t k;
    int id;
    k = ftok("/dev/null", '2');
    if(k == -1) perror("ftok");
    id = msgget(k, 0);
    if(id == -1) perror("msgget");
    return id;
}
void remove_queue(int id){
    int r;
    r = msgctl(id, IPC_RMID, NULL);
    if(r == -1) perror("msgctl");
}
int write_haiku(category c, haiku *h){
    int id = create_queue();
    if(id == -1) return -1;
    assert(h);
    struct haiku_msg m = {0};
    m.type = c+1; m.package = *h;
    int r = msgsnd(id, &m, sizeof(m) - sizeof(m.type), 0);
    if(r == -1) perror("msgsnd");
    return r;
}
int read_haiku(category c, haiku *h){
    int id = access_queue();
    if(id == -1) return -1;
    assert(h);
    struct haiku_msg m = {0};
    int r = msgrcv(id, &m, sizeof(m) - sizeof(m.type), c+1, IPC_NOWAIT);
    //if(r == -1) perror("msgrcv");
    *h = m.package;
    return  r;
}


