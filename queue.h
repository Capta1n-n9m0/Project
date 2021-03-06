#ifndef PROJECT_QUEUE_H
#define PROJECT_QUEUE_H
#include "haiku.h"


struct haiku_msg{
    long type;
    haiku package;
};

int create_queue();
int access_queue();
int remove_queue(int id);
int write_haiku(category c, haiku *h);
int read_haiku(category c, haiku *h);


#endif //PROJECT_QUEUE_H
