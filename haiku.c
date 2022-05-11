#define  _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "haiku.h"

//
file_category haiku_files[] = {
        {japanese, "japanese.haiku"},
        {western, "western.haiku"}
};

// function that would read
book read_book(category c){
    char *filename;
    filename = haiku_files[c].file;
    book res = {0};
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *f = fopen(filename, "r");
    if(f == NULL) {
        fprintf(stderr, "failed to open %s\n", filename);
        exit(1);
    }
    while ( (read = getline(&line, &len, f)) != -1) {
        if(line[0] == '\n' || line[0] == '\r') break;
        res.size++;
        if(res.size == 1)
            res.poems = calloc(1, sizeof(haiku));
        else
            res.poems = realloc(res.poems, sizeof(haiku)*res.size);
        line[read-1] = '\0';
        strncpy(res.poems[res.size-1].author, line, 64);
        if((read = getline(&line, &len, f)) != -1) {
            line[read-1] = '\0';
            strncpy(res.poems[res.size - 1].lines[0], line, 64);
        }
        else exit(1);
        if((read = getline(&line, &len, f)) != -1) {
            line[read-1] = '\0';
            strncpy(res.poems[res.size - 1].lines[1], line, 64);
        }
        else exit(1);
        if((read = getline(&line, &len, f)) != -1) {
            line[read-1] = '\0';
            strncpy(res.poems[res.size - 1].lines[2], line, 64);
        }
        else exit(1);
    }
    fclose(f);
    if (line)
        free(line);
    return res;
}

haiku select_random(book b){
    int choice = rand() % b.size;
    return b.poems[choice];
}

void print_haiku(haiku h){
    printf("Author: %s\n", h.author);
    printf("%s\n", h.lines[0]);
    printf("%s\n", h.lines[1]);
    printf("%s\n", h.lines[2]);
}

void free_book(book *b){
    free(b->poems);
    b->size = 0;
}