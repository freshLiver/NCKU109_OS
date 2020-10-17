// TODO: COMPARE fscanf, getline, fstream >>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <queue>
#include <string>


int main ( int argc, char const *argv[] ) {

    clock_t start = clock ( );
    std::queue< char * > result;

    // std::getline use ifstream
    FILE *fin = fopen ( argv[1], "r" );

    // read
    int count;
    char tmp[250];
    for ( count = 0; EOF != fscanf ( fin, "%s", tmp ); ++count )
        result.push ( tmp );
    fclose ( fin );

    printf ( "read %d cost : %lf<<\n", count, ( double ) ( clock ( ) - start ) );
    return 0;
}
