#include "KeyValueStorage.h"
#include <ctime>

/**
 * @brief Convert input file name to output file name by change .input -> .output
 *
 * @param inputName input filename with extension(.input)
 * @return string return output filename with extension(.output)
 */
string GetOutputName( string inputName ) {

    string filename = "";

    for ( int in = 0, out = 0; inputName[in] != '\0'; ++in, ++out ) {
        if ( inputName[in] == '/' )
            filename = "";
        else
            filename += inputName[in];
    }

    string outputName = filename.substr( 0, filename.length() - 5 ) + "output";

    return outputName;
}

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */
int main( int argc, char const *argv[] ) {

    try {
        time_t start, finish;

        // start counting time
        start = time( NULL );

        // get input and output file name with ext as std::string
        string inputFilename( argv[1] );
        string outputFileName( GetOutputName( inputFilename ) );

        // pass input and output to KVS instance
        KeyValueStorage( inputFilename, outputFileName );

        // get finish time, and calc time cost
        finish = time( NULL );
        printf( "Total Time Cost : %ld secs.\n", finish - start );
    }
    catch ( std::exception &e ) {
        printf( "err : %s\n", e.what() );
    }

    return 0;
}
