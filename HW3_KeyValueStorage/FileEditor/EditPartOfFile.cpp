#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

using namespace std;

void ReplaceAllEMPTY( string file ) {
    FILE *pRead = fopen( file.c_str(), "r+" );

    char line_buf[160];

    for ( ; EOF != fscanf( pRead, "%s", line_buf ); ) {
        if ( line_buf[0] == 'E' && line_buf[4] == 'Y' ) {
            // return \n pos
            long begin = ftell( pRead );
            auto res = fseek( pRead, -5, SEEK_CUR );
            fputs( "EEEEE", pRead );
            fflush( pRead );
        }
    }
    fclose( pRead );
}

void AppendToEnd( string file ) {
    FILE *pApp = fopen( file.c_str(), "a+" );

    fputs( "TEEEEE", pApp );
    fflush( pApp );
    fclose( pApp );
}

void WriteFromFileEnd( string file ) {
    fstream fs( file.c_str(), ios_base::app );
    fs.write( "++==+", 5 );
    fs.close();
}

int main( int argc, char const *argv[] ) {
    //
    AppendToEnd( "test.txt" );
    return 0;
}
