#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include <pthread.h>


void server_v1();
void v2_haiku_writer();
void v2_haiku_reader();
void server_v3();
void *writer_thread_function(void *arg);
void *reader_thread_function(void *arg);




#endif //PROJECT_SERVER_H
