#include "def.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>

int main ( int argc, char const *argv[] ) {

    // max size string
    std::string cell = MAX_CELL;

    // init string queue
    int size = 4943342;
    std::string *results = new std::string[size];

    for ( int i = 0; i < size; ++i )
        results[i] = MAX_CELL;

    // max 450 words without format
    clock_t start = clock ( );
    const char output[] = "/home/freshliver/Tmp/sample.output";

    FILE *fout = fopen ( output, "w" );
    for ( int i = 0; i < size; ++i )
        fprintf ( fout, "%s", results[i].c_str() );
    fflush ( fout );
    fclose ( fout );

    printf ( "%d cells cost : %lf \n", size, ( double ) ( clock ( ) - start ) );
    return 0;
}
