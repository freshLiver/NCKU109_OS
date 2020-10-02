#include "SortLargeFile.h"
#include <cstdio>
#include <ctime>

int main ( int argc, char const *argv[] ) {

    time_t start = time ( NULL );
    SortLargeFile test ( argv[1], argv[2] );
    time_t end = time ( NULL );

    printf ( "Total Cost : %ld\n", end - start );
    return 0;
}
