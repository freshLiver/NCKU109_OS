// Example program
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int ChooseFirstChar( string cmd ) { return cmd[( cmd[0] == 'S' ) ? 5 : 4] - '0'; }

int ChooseLastChar( string cmd ) {
    int kStart = ( cmd[0] == 'S' ) ? 5 : 4;
    for ( ;; ++kStart )
        if ( cmd[kStart] == ' ' || cmd[kStart] == '\0' )
            return cmd[kStart - 1] - '0';
    return -1;
}

int main() {
    int size = 100000;
    string buffer[size];
    fstream fin( "./test.input", ios::in );
    for ( int i = 0; i < size; ++i ) {
        getline( fin, buffer[i] );
    }

    // choose first char
    int res;
    for ( int i = 0; i < size; ++i ) {
        res = ChooseFirstChar( buffer[i] );
    }

    return 0;
}
