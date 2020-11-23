#include "macros.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
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

class KeyValueStorage {
private:
    /* data */
public:
    //
    // thread putThreads[10];
    // queue< tuple< LL, char[128] > > putCommands[10];
    KeyValueStorage ( string input );

    static void ReadInputFile ( string input ) {
        std::ifstream fin ( input.c_str ( ), std::ios::in );
        int count = 1;
        for ( string tmp; std::getline ( fin, tmp ); ++count ) {
            int type, index;
            string key, value;
            auto result = ParseCommand ( tmp );
            DEBUG ( "line[%3d] : type: %d, index: %d\nkey: %s\nvalue: %s\n",
                    count,
                    std::get< 0 > ( result ),
                    std::get< 1 > ( result ),
                    std::get< 2 > ( result ).c_str ( ),
                    std::get< 3 > ( result ).c_str ( ) );
        }
    }

    static tuple< int, int, string, string > ParseCommand ( string cmd ) {
        int type, index;
        char key[20], value[129] = "\0";

        // parse command type
        int iCmd = 4;
        if ( cmd[0] == 'P' )
            type = PUT;
        else if ( cmd[0] == 'G' )
            type = GET;
        else {
            type = SCAN;
            iCmd = 5;
        }

        // read key and key index
        for ( int iKey = 0;; ++iKey, ++iCmd ) {
            // command format : "TYPE KEY VALUE"
            if ( cmd[iCmd] == ' ' || cmd[iCmd] == '\0' ) {
                // last char is index
                index = cmd[iCmd - 1] - '0';
                // make string end
                key[iKey] = '\0';
                ++iCmd;
                break;
            }
            key[iKey] = cmd[iCmd];
        }

        if ( type == PUT ) {
            // read value
            for ( int iValue = 0; iValue < 128; ++iValue, ++iCmd )
                value[iValue] = cmd[iCmd];
            value[128] = '\0';
        }
        return tuple< int, int, string, string > ( type, index, string ( key ), string ( value ) );
    }
};
