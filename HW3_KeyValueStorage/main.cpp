#include "KeyValueStorage.h"
#include <ctime>


/**
 * @brief Convert input file name to output file name by change .input -> .output
 * 
 * @param inputName input filename with extension(.input)
 * @return string return output filename with extension(.output)
 */
string GetOutputName( string inputName ) {
    string res( inputName.size() + 1, '\0' );
    int dotEnd = inputName.size() - 5;

    // copy string before "input"
    for ( int i = 0; i < dotEnd; ++i )
        res[i] = inputName[i];

    // copy ".input" and right shift 1 char
    for ( int i = dotEnd; i < res.size(); ++i )
        res[i] = inputName[i - 1];

    // .in -> out
    res[dotEnd++] = 'o';
    res[dotEnd++] = 'u';
    res[dotEnd++] = 't';

    return res;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main( int argc, char const *argv[] ) {
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

    return 0;
}
