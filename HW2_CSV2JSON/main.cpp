#include "CSV2JSON.h"

#define OUTPUT_PATH "/home/freshliver/Tmp/output.json"

typedef unsigned int uint;

/**
 * convert char * string into uint32
 * */
static uint c_str2int ( const char *c_str ) {
    // if null
    if ( c_str == NULL )
        return 1;

    uint value = 0;
    // convert to int
    for ( int i = 0; c_str[i] != '\0'; ++i ) {
        value *= 10;
        value += ( c_str[i] - '0' );
    }

    return ( value == 0 ) ? 1 : value;
}


/**
 * Main Function
 * */
int main ( int argc, char const *argv[] ) {
    // time counting
    time_t start = time ( NULL );

    /** handle args
     *  @ argv[1] : input file path
     *  @ argv[2] : number of threads
     * */

    string input ( argv[1] );
    string output ( OUTPUT_PATH );

    uint threads = c_str2int ( argv[2] );

    DEBUG ( "I/O Path : %s -> %s", input.c_str ( ), output.c_str ( ) );
    DEBUG ( "threads  : %d", threads );

    // read and initialize class with datas and threads
    CSV2JSON cvs2json ( input, output, ( threads == 1 ) ? 1 : ( threads - 1 ) );
    COST_TIME ( "csv2json total cost :" );

    return 0;
}
