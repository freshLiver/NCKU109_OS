#include <bits/stdc++.h>

using namespace std;

#define BufSize 1000

pair<streampos, string> GetLineEnd( fstream &db, string key ) {
    // read pointer move to beg
    db.seekg( 0, ios_base::beg );

    // search whole file
    bool found = false;

    streampos lineEnd = 0;
    string lineCmd;

    string *buffer = new string[BufSize];

    while ( db.eof() == false && !found ) {
        // read into buffer
        int count = 0;
        for ( ; count < BufSize; ++count )
            getline( db, buffer[count] );

        // try to find key in buffer
        for ( int iBuffer = 0; iBuffer < count && !found; ++iBuffer ) {
            // ! move to line end should include \n
            lineEnd += ( buffer[iBuffer].length() + 1 );

            // check key of this line
            bool sameKey = true;
            for ( int iKey = 0; sameKey && ( key[iKey] != '\0' ); ++iKey )
                sameKey = ( key[iKey] == buffer[iBuffer][iKey] );

            // if same key, set found
            if ( sameKey == true ) {
                found = true;
                lineCmd = buffer[iBuffer];
            }
        }
    }

    // free buffer
    db.clear();
    delete[] buffer;

    // determine real line end
    if ( found == false )
        return make_pair( -1, "EMPTY" );

    return make_pair( lineEnd, lineCmd );
}

int main( int argc, char const *argv[] ) {

    fstream fin( "./db0" );
    auto res = GetLineEnd( fin, "993798708473132710" );

    //
    fin.seekp( res.first - 6, ios_base::beg );
    fin.write( "<<<<<", 5 );
    fin.close();
    return 0;
}
