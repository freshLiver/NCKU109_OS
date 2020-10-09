#include "CSV2JSON.h"
#include <ctime>
#include <fstream>

/**
 * Convert char[] to int
 * */
static int CStr2Int32 ( const char *str ) {
    int result;

    return result;
}


/**
 * read csv file and store to queue< pair<int,string> >
 * */
static int ReadCSV ( queue< pair< int, string > > &container, const char *target ) {
    // use std::getline
    std::ifstream fin ( target, std::ios::in );

    int count = 0;
    for ( string tmp; std::getline ( fin, tmp ); ++count )
        container.push ( pair< int, string > ( count, tmp ) );

    // close file
    fin.close ( );

    // result total lines
    return count;
}


/**
 * write every Value in Values[] into JSON file
 * */
static void WriteJSON ( pValues results ) {}

int main ( int argc, char const *argv[] ) {
    
    // count from program start
    time_t start;

    // default 1 thread
    int threads = ( argv[2] == NULL ) ? 1 : CStr2Int32 ( argv[2] );

    // print infos
    DEBUG ( "Input File : %s", argv[1] );
    DEBUG ( " USE %d Threads ", threads );
    DEBUG ( "Output File : %s", argv[3] );


    // read file and store to CSV2JSON::datas
    RESET_TIME ( );
    int count = ReadCSV ( CSV2JSON::datas, argv[1] );
    COST_TIME ( "Read CSV File" );

    // build class
    CSV2JSON cvs2json ( count, threads );

    // write every Value in CSV2JSON::result into JSON file
    RESET_TIME ( );
    WriteJSON ( CSV2JSON::results );
    COST_TIME ( "Write JSON File" );

    return 0;
}
