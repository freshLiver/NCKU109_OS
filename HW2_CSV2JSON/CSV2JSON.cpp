#include "CSV2JSON.h"

// CTOR

CSV2JSON::CSV2JSON ( int totalLines, short threads ) {
    // set total cvs lines and multi-thread mode
    CSV2JSON::lines = totalLines;
    CSV2JSON::threads = threads;

    // create $(lines) cells to store result
    CSV2JSON::results = new Values[totalLines];
}

// methods
void CSV2JSON::ParseLineStoreValues ( ) {
    // async get foront data in raw data queue
    future< pair< int, string > > data = async ( CSV2JSON::AsyncPopQueue );

    int key = data.get ( ).first;
    string str = data.get ( ).second;


    // TODO HANDLE THIS DATA
    int values[20];

    // start from str end, find all digit and minus symbol
    for ( int p = str.length ( ) - 1, index = 19, weight = 1; p >= 0; --p ) {
        // move to next index and reset weight to 1
        if ( str[p] == '|' ) {
            weight = 1;
            --index;
        }

        // char to int, * weight
        else if ( '0' <= str[p] && str[p] <= '9' ) {
            values[index] += weight * ( str[p] - '0' );
            weight *= 10;
        }

        // value * -1
        else if ( str[p] == '-' )
            values[index] *= -1;

        // should not have other char
        else
            DEBUG ( "find >>%c<<", str[p] );
    }

    // TODO DEBUG
    DEBUG ( "[%d] : %s", key, str.c_str ( ) );
    for ( int i = 0; i < 20; ++i )
        printf ( "%d|", values[i] );

    // STORE INTO CORRESPONDING POS IN CSV2JSON::results
    CSV2JSON::results[key] = Values ( values );
}
