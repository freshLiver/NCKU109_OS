#include <cstdio>
#include <cstdlib>
#include <ctime>

void bufout ( FILE *output, int *buffer, int &count ) {
    for ( int i = 0; i < count; ++i )
        fprintf ( output, "%d\n", buffer[i] );
    count = 0;
}

int main ( ) {
    long start = time ( NULL );

    FILE *input = fopen ( "/home/freshliver/Tmp/OS_TEST/A/big_file.txt", "r" );
    FILE *output = fopen ( "bufferOn.txt", "w" );

    // new buffer
    const int bufferSize = 10000000;
    int *buffer = new int[bufferSize];

    // copy first 1000w lines
    int count = 0;
    for ( int i = 0; i < 50000000; ++i ) {
        if ( count >= bufferSize )
            bufout ( output, buffer, count );

        fscanf ( input, "%d", &buffer[count++] );
    }
    bufout ( output, buffer, count );


    // close file
    fflush ( output );
    fclose ( input );
    fclose ( output );

    // cost time
    printf ( "cost time : %ld s.\n", time ( NULL ) - start );
    return 0;
}
