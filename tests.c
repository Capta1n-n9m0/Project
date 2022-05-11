#include "tests.h"
#include "haiku.h"
#include <stdlib.h>
#include <stdbool.h>

bool compare_books(book b1, book b2){

}


int test(){
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


