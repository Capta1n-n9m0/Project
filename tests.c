#include "haiku.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool compare_haikus(haiku h1, haiku h2){
    if(strcmp(h1.author, h2.author)) return false;
    for(int i = 0; i < 3; i++)
        if(strcmp(h1.lines[i], h2.lines[i])) return false;
    return true;
}

bool compare_books(book b1, book b2){
    if(b1.size != b2.size) return false;
    for (int i = 0; i < b1.size; i++)
        if(!compare_haikus(b1.poems[i], b2.poems[i])) return false;
    return true;
}


void test(){
    book b;
    b.size = 2;
    b.poems = calloc(2, sizeof(haiku));
    b.poems[0] = (haiku){
            "Author",
            {
                "line1",
                "line2",
                "line3"
            }
    };
    b.poems[1] = (haiku){
            "Author",
            {
                "line1",
                "line2",
                "line3"
            }
    };
    book actuall = read_book(japanese);
    compare_books(b, actuall);

}


