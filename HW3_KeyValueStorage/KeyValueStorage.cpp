#include "KeyValueStorage.h"

using std::get;
using std::make_tuple;

/***********************************************
**************** public methods ****************
***********************************************/


KeyValueStorage::KeyValueStorage( string input, string output ) {

    // get input and output file name
    KeyValueStorage::inputFile = input;
    KeyValueStorage::outputFile = output;

    // 檢查是否所有 db 檔案都存在，若不存在就需要建立檔案
    system( "mkdir ./db/" );
    for ( int dbID = 0; dbID < 10; ++dbID ) {
        string touchCmd = "touch ./db/db0";
        touchCmd[touchCmd.length() - 1] = '0' + dbID;
        system( touchCmd.c_str() );
    }

    // 初始化所有 worker 和 thread
    for ( int id = 0; id < 10; ++id ) {

    }
}


/************************************************
**************** private methods ****************
************************************************/


int KeyValueStorage::GetKeyFromCmd( string &cmd, string &key, int start, char delim ) {
    for ( int iCmd = start, iKey = 0;; ++iCmd, ++iKey ) {
        if ( cmd[iCmd] == delim )
            return ++iCmd;
        else
            key[iKey] = cmd[iCmd];
    }
}


void KeyValueStorage::GetValueFromCmd( string &cmd, string &value, int start ) {
    for ( int iCmd = start, iValue = 0; iValue < 128; ++iCmd, ++iValue )
        value[iValue] = cmd[iCmd];
}


tuple<string, string> KeyValueStorage::ParseCommandAs( CmdType type, string cmd ) {

    string key( 20, '\0' ), value( 129, '\0' );
    int end;
    switch ( type ) {
        // * GET  : GET key[<20]
        case PUT:
            end = GetKeyFromCmd( cmd, key, 4, ' ' );
            GetValueFromCmd( cmd, value, end );
            break;

        // * PUT  : PUT key[<20] value[128]
        case GET:
            GetKeyFromCmd( cmd, key, 4, '\0' );
            break;

        // * SCAN : SCAN key1[<20] key2[<20]
        case SCAN:
            end = GetKeyFromCmd( cmd, key, 5, ' ' );
            GetKeyFromCmd( cmd, value, end, '\0' );
            break;
    }
    return make_tuple( key, value );
}


tuple<CmdType, string, string> KeyValueStorage::ParseCommand( string cmd ) {

    CmdType type;
    // parse command type
    switch ( cmd[0] ) {
        case 'P':
            type = PUT;
            break;
        case 'G':
            type = GET;
            break;
        case 'S':
            type = SCAN;
            break;
        default:
            throw new std::runtime_error( "Invalid Command, not PUT, GET or SCAN.\n" );
    }
    auto info = ParseCommandAs( type, cmd );
    return make_tuple( type, get<0>( info ), get<1>( info ) );
}
