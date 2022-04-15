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
    k = ftok("/dev/null", 'H');
    if(k == -1) perror("ftok");
    id = msgget(k, IPC_CREAT | 0666);
    if(id == -1) perror("msgget");
    return id;
}
int access_queue(){
    key_t k;
    int id;
    k = ftok("/dev/null", 'H');
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
    int id;
    if(!is_queue){
        id = create_queue();
        is_queue = 1;
    } else id = access_queue();
    assert(h);
    int r;
    struct haiku_msg m = {0};
    m.type = c;
    m.package = *h;
    r = msgsnd(id, &m, sizeof(m) - sizeof(m.type), 0);
    if(r == -1) perror("msgsnd");
    return r;
}
int read_haiku(category c, haiku *h){
    int id = access_queue();
    if(id == -1) return -1;
    assert(h);
    int r;
    struct haiku_msg m = {0};
    r = msgrcv(id, &m, sizeof(m) - sizeof(m.type), c, 0);
    if(r == -1) perror("msgrcv");
    *h = m.package;
    return  r;
}


