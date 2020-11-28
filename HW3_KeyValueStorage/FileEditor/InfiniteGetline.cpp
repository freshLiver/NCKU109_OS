#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

void InfRead( fstream &fin, int count ) {
    for ( string tmp; getline( fin, tmp ); )
        printf( "%d %s\n", count, tmp.c_str() );
}

int main( int argc, char const *argv[] ) {
    fstream fin( "./test.txt" );
    InfRead( fin, 0 );
    InfRead( fin, 1 );
    fin.clear();
    fin.seekg( 0, ios::beg );
    InfRead( fin, 2 );
    return 0;
}
