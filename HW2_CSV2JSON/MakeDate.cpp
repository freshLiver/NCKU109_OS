#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>

using std::string;
typedef long long LL;

#define INTMIN -2147483648
#define UINTMAX ( LL ) 4294967296

string Random1Line ( ) {
    string thisLine = "";

    for ( int i = 0; i < 20; ++i ) {
        int value = rand ( ) % ( 2 * ( UINTMAX ) ) + INTMIN;
        char v2s[11];
        sprintf ( v2s, "%d%c", value, ( i != 19 ) ? '|' : '\n' );
        thisLine.append ( v2s );
    }
    return thisLine;
}


int main ( int argc, char **argv ) {
    // random seed
    srand ( time ( NULL ) );

    int lines = 8659200;
    FILE *output = fopen ( "./input.cvs", "w" );
    for ( int line = 0; line < lines; ++line ) {
        fprintf ( output, "%s", Random1Line ( ).c_str ( ) );
    }
    return 0;
}
