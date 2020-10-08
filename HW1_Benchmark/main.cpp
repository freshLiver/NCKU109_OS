#include "SortLargeFile.h"
#include <ctime>


static const char *GetFileFolder ( const char *file ) {
    string tmp ( file );
    int lastSlash = tmp.rfind ( "/" );

    char *result = new char[200];
    strcpy ( result, tmp.substr ( 0, lastSlash ).c_str ( ) );
    return result;
}

static void MoveFile ( const char *src, const char *dest ) {
    char cmd[200];
    sprintf ( cmd, "mv %s %s", src, dest );
    system ( cmd );
}

int main ( int argc, char const *argv[] ) {

    // program start, start counting and deal with args
    time_t start = time ( NULL );
    const char *cwd = GetFileFolder ( argv[1] );

    try {
        char cmd[100];
        sprintf ( cmd, "mkdir %s/outputs", cwd );
        system ( cmd );
    }
    catch ( std::exception ex ) {
        std::cout << ex.what ( ) << std::endl;
    }

    // sort file
    SortLargeFile test ( argv[1], ( argv[2] != NULL ) ? argv[2] : cwd );

    // sort end, move result file
    char result[200];
    sprintf ( result, "%s/output.txt", cwd );
    MoveFile ( test.outputFile.c_str ( ), result );

    // program end
    time_t end = time ( NULL );
    printf ( "Total Cost : %ld secs\n", end - start );
    return 0;
}
