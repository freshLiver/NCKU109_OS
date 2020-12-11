#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

#define SIZE 100000

int res;

int ChooseFirstChar( string cmd ) {
    //
    return cmd[( cmd[0] == 'S' ) ? 5 : 4] - '0';
}

void ReadIntoBuffer( fstream &fin ) {
    string *cmds = new string[SIZE];

    // read into string buffer
    for ( int i = 0; i < SIZE; ++i ) {
        getline( fin, cmds[i] );
    }

    // parse cmds
    for ( int i = 0; i < SIZE; ++i ) {
        res = ChooseFirstChar( cmds[i] );
    }
}

void ReadAndExecute( fstream &fin ) {

    // parse cmds
    string cmd;
    for ( int i = 0; i < SIZE; ++i ) {
        getline( fin, cmd );
        res = ChooseFirstChar( cmd );
        printf( "%d:%s\n", res, cmd.c_str() );
    }
}

int main( int argc, char const *argv[] ) {
    /* code */
    string file( "./../test.input" );
    fstream fin( file.c_str(), ios_base::in );

    // ReadIntoBuffer( fin );
    ReadAndExecute( fin );

    return 0;
}
