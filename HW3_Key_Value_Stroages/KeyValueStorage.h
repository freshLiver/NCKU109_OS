#include "macros.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fstream>
#include <queue>
#include <string>
#include <thread>
#include <tuple>

using std::fstream;
using std::queue;
using std::string;
using std::thread;
using std::tuple;
using std::get;

class KeyValueStorage {
private:
    static int GetKeyFromCmd ( string &cmd, string &key, int start, char delim ) {
        for ( int iCmd = start, iKey = 0;; ++iCmd, ++iKey ) {
            if ( cmd[iCmd] == delim )
                return ++iCmd;
            else
                key[iKey] = cmd[iCmd];
        }
    }
    static void GetValueFromCmd ( string &cmd, string &value, int start ) {
        for ( int iCmd = start, iValue = 0; iValue < 128; ++iCmd, ++iValue )
            value[iValue] = cmd[iCmd];
    }
    //
    // Parse This Command As Given CmdType
    //
    static tuple<string, string> ParseCommandAs ( CmdType type, string cmd ) {
        string key ( 20, '\0' ), value ( 129, '\0' );
        int end;
        switch ( type ) {
            // * GET  : GET key[<20]
            case PUT:
                end = GetKeyFromCmd ( cmd, key, 4, ' ' );
                GetValueFromCmd ( cmd, value, end );
                break;

            // * PUT  : PUT key[<20] value[128]
            case GET:
                GetKeyFromCmd ( cmd, key, 4, '\0' );
                break;

            // * SCAN : SCAN key1[<20] key2[<20]
            case SCAN:
                end = GetKeyFromCmd ( cmd, key, 5, ' ' );
                GetKeyFromCmd ( cmd, value, end, '\0' );
                break;
        }
        return std::make_tuple ( key, value );
    }

public:
    //
    // thread putThreads[10];
    // queue< tuple< LL, char[128] > > putCommands[10];
    KeyValueStorage ( string input );


    //
    // Read Input File
    // TODO: Read N lines once, until EOF
    //
    static void ReadInputFile ( string input ) {
        std::ifstream fin ( input.c_str ( ), std::ios::in );
        int count = 1;
        for ( string tmp; std::getline ( fin, tmp ); ++count ) {
            int type, index;
            string key, value;
            auto res = ParseCommand ( tmp );
            DEBUG ( "line[%3d](%d) :\n\tkey: %s\n\tvalue: %s\n",
                    count,
                    get<0> ( res ),
                    get<1> ( res ).c_str ( ),
                    get<2> ( res ).c_str ( ) );
        }
    }
    //
    // Parse This Command, and return its informations
    //
    static tuple<CmdType, string, string> ParseCommand ( string cmd ) {
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
                throw new std::runtime_error ( "Invalid Command, not PUT, GET or SCAN.\n" );
        }
        auto info = ParseCommandAs ( type, cmd );
        return std::make_tuple ( type, get<0> ( info ), get<1> ( info ) );
    }
};
