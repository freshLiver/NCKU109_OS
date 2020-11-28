#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

bool ReadAfterEOF( const char *path ) {
    fstream file( path );
    for ( string tmp; getline( file, tmp ); )
        ;
    file.seekg( 0 );
    return file.eof();
}

int main( int argc, char const *argv[] ) {
    printf( "%s\n", ReadAfterEOF( "./test.txt" ) ? "Ture" : "False" );
    return 0;
}
