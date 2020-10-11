#include "CSV2JSON.h"

uint CSV2JSON::workers;
uint CSV2JSON::lines;
string *CSV2JSON::cells;
queue< pair< uint, string > > *CSV2JSON::queues;


CSV2JSON::CSV2JSON ( string input, string output, uint workers ) {
    // how many workers
    CSV2JSON::workers = workers;
    CSV2JSON::queues = new queue< pair< uint, string > >[workers];

    // read whole CSV and store queues
    CSV2JSON::lines = CSV2JSON::ReadCSV ( input, workers );

    // dynamic alloc result list
    CSV2JSON::cells = new string[CSV2JSON::lines];

    // ! Threading Parse Datas
    if ( workers == SINGLE_THREAD ) {
        CSV2JSON::ThreadingParseDatas ( 0 );
    }
    else {
        // create workers and assign task
        thread ths[workers];
        for ( int i = 0; i < workers; ++i )
            ths[i] = thread ( CSV2JSON::ThreadingParseDatas, i );

        // wait all worker
        for ( int i = 0; i < workers; ++i )
            ths[i].join ( );
    }
    DEBUG ( "Threading Parse End %s", "<<<<" );

    // TODO Write into JSON
    FILE *out = fopen ( output.c_str ( ), "w" );
    for ( int i = 0; i < CSV2JSON::lines; ++i )
        fprintf ( out, "%s", CSV2JSON::cells[i].c_str ( ) );
    fflush ( out );
    fclose ( out );
}

//
// parse line and store values
//
void CSV2JSON::ThreadingParseDatas ( uint worker_id ) {
    // get target queue (based on worker_id)
    queue< pair< uint, string > > *myqueue = &CSV2JSON::queues[worker_id];

    // ! do unil target queue
    while ( myqueue->empty ( ) == false ) {
        // get front data (line) in target queue and pop it
        pair< uint, string > kv = myqueue->front ( );
        myqueue->pop ( );

        // split 2 string list
        string list[Numpl];
        Split2List ( kv.second, list );

        // string to formated json cell
        CSV2JSON::cells[kv.first].assign ( CSV2JSON::List2Cell ( list ) );
    }
}
//
// split a string has 20 numbers that split by '|' into string[]
//
void CSV2JSON::Split2List ( string raw, string *list ) {
    short start = 0, end = 0, index = 0;
    for ( ; end < raw.length ( ); ++end ) {
        // '0' ~ '9' or '-'
        if ( raw[end] != '|' && raw[end] != '\0' )
            continue;
        // '|' or '\0' : number end, raw[start:i] is this number
        else {
            // get this number
            list[index] = raw.substr ( start, end - start );
            // set start and move to next list element
            ++index;
            start = end + 1;
        }
    }
    // get last number
    list[Numpl - 1] = raw.substr ( start, end - start );
}
//
// turn the result of CSV2JSON::Split2List into json format that is defined in "macros.h"
//
string CSV2JSON::List2Cell ( string *list ) {
    char cell[550];
    sprintf ( cell, SUPER_BRUTE_FORCE_JSON_FORMAT, SUPER_BRUTE_FORCE_FORMAT_PARAMS );
    return cell;
}
