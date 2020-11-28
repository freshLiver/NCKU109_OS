#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

using namespace std;

void ReplaceAllEMPTY( string file ) {
    fstream fs( file.c_str() );
    for ( string tmp; getline( fs, tmp ); ) {
        if ( tmp[0] == 'E' && tmp[4] == 'Y' ) {
            long pos = fs.tellp() - 6;
            fs.seekp( pos );
            fs.write( "TEST ", 5 );
        }
    }
    fs.close();
}

void WriteFromFileEnd( string file ) {
    fstream fs( file.c_str(), ios_base::app );
    fs.write( "++==+", 5 );
    fs.close();
}

int main( int argc, char const *argv[] ) {
    //
    WriteFromFileEnd( "test.txt" );
    return 0;
}
