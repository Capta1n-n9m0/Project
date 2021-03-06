#ifndef PROJECT_HAIKU_H
#define PROJECT_HAIKU_H

// structure describing haiku
// haiku consists of its author and three lines of poems
typedef struct haiku_{
    char author[64];
    char lines[3][64];
} haiku;

// book is a dynamic array of haikus it stores size of the array and pointer to start of the array
typedef struct book_{
    int size;
    haiku *poems;
}book;

// category is just an enum for convenient storage of haiku categories
typedef enum category_{
    japanese,
    western
} category;

// file_category structure was created for single purpose: create link between category of a haiku and file where this haiku is stored
typedef struct file_category_{
    category c;
    char file[64];
} file_category;

// return the book of a haiku formed from file. filenames are hardcoded using array of file_category structure
book read_category(category c);
book read_book(const char *filename);
haiku select_random(book b);
void print_haiku(haiku h);
void free_book(book *b);
// returns random haiku from a given book
// displays given haiku in terminal
// because book works with dynamic memory, it needs to be freed

#endif //PROJECT_HAIKU_H
