#include "../macros.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <tuple>

using namespace std;


tuple<CmdType, int> QuickParseCmd( string &cmd ) {
    int keyPos;
    CmdType type;
    // get key start position and cmd type
    switch ( cmd[0] ) {
        case 'P':
            keyPos = 4;
            type = PUT;
            break;

        case 'G':
            keyPos = 4;
            type = GET;
            break;

        case 'S':
            keyPos = 5;
            type = SCAN;
            break;
    }

    // get char in order until space(key end)
    char lastChar;
    for ( int iCmd = keyPos;; ++iCmd )
        if ( cmd[iCmd] == ' ' || cmd[iCmd] == '\0' ) {
            lastChar = cmd[iCmd - 1];
            break;
        }

    // 回傳型別以及最後一個數字
    return make_tuple( type, lastChar - '0' );
}

int main( int argc, char const *argv[] ) {
    //
    string cmd = "GUT 4234237243";
    auto res = QuickParseCmd( cmd );
    return 0;
}
