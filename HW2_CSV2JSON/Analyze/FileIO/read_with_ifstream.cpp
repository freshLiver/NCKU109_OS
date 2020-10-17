// TODO: COMPARE fscanf, getline, fstream >>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <queue>
#include <string>


int main ( int argc, char const *argv[] ) {

    clock_t start = clock ( );
    std::queue< std::string > result;

    // std::getline use ifstream
    std::ifstream fin ( argv[1], std::ios::in );

    // read
    int count;
    std::string tmp;
    for ( count = 0; fin >> tmp; ++count )
        result.push ( tmp );
    fin.close ( );

    printf ( "cost : %lf<<\n", ( double ) ( clock ( ) - start ) );
    return 0;
}
