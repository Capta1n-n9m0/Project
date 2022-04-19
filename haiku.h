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
book read_book(category c);
// returns random haiku from a given book
haiku select_random(book b);
// displays given haiku in terminal
void print_haiku(haiku h);
// because book works with dynamic memory, it needs to be freed
void free_book(book *b);

#endif //PROJECT_HAIKU_H
