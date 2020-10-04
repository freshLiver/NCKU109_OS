#include <cstdio>
#include <cstdlib>
#include <ctime>

int main ( ) {
    long start = time ( NULL );

    FILE *input = fopen ( "/home/freshliver/Tmp/OS_TEST/A/big_file.txt", "r" );
    FILE *output = fopen ( "output.txt", "w" );

    // copy first 1000w lines
    int value;
    for ( int count = 0; count < 10000000; ++count ) {
        fscanf ( input, "%d", &value );
        fprintf ( output, "%d\n", value );
    }


    // close file
    fflush ( output );
    fclose ( input );
    fclose ( output );

    // cost time
    printf ( "cost time : %ld s.\n", time ( NULL ) - start );
    return 0;
}
