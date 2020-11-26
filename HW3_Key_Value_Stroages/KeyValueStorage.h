#include "macros.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <queue>
#include <string>
#include <thread>
#include <tuple>

using std::get;
using std::make_tuple;
using std::queue;
using std::string;
using std::thread;
using std::tuple;

class KeyValueStorage {

public:
    //
    // ctor :
    // * 1. get input and output file name
    // * 2. check if db files exist and init dbFiles[]
    // * 3.
    //
    KeyValueStorage( string input, string output );
    //
    // Read N Lines At Once From Input File, Until EOF
    //
    static void ReadCommands();
    //
    // should parse command in cmdQueue and push to resp until GET or SCAN while Reading
    //
    static void ParseOnReading();

private:
    //
    static string inputFile, outputFile;
    static thread dbWorkers[10];
    static std::ofstream dbFiles[10];
    static queue<tuple<string, string>> workerCmds[10];

    //
    //
    //
    static int GetKeyFromCmd( string &cmd, string &key, int start, char delim ) {
        for ( int iCmd = start, iKey = 0;; ++iCmd, ++iKey ) {
            if ( cmd[iCmd] == delim )
                return ++iCmd;
            else
                key[iKey] = cmd[iCmd];
        }
    }
    //
    //
    //
    static void GetValueFromCmd( string &cmd, string &value, int start ) {
        for ( int iCmd = start, iValue = 0; iValue < 128; ++iCmd, ++iValue )
            value[iValue] = cmd[iCmd];
    }
    //
    // Parse This Command As Given CmdType
    //
    static tuple<string, string> ParseCommandAs( CmdType type, string cmd ) {
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
    //
    // Parse This Command, and return its informations
    //
    static tuple<CmdType, string, string> ParseCommand( string cmd ) {
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
};
