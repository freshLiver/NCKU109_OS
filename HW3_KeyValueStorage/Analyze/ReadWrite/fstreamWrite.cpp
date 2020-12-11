#include <bits/stdc++.h>

using namespace std;

void NoIOWholeFile( string output ) {
    //
    fstream fout( output.c_str(), ios_base::out | ios_base::in | ios_base::ate );

    string sample = "this is a sample string";
    fout.seekp( 12, ios_base::beg );
    fout.write( sample.c_str(), sample.length() );
    fout.flush();
    fout.close();
}

void IOWholeFile( string output ) {
    //
    ifstream fin( output.c_str(), ios_base::in | ios_base::ate );
    streamsize size = fin.tellg();
    fin.seekg( 0, ios_base::beg );
    char *content = new char[size];
    fin.readsome( content, size );
    fin.close();

    char *pChar = &content[11];
    string replace = ">>AAAAAAAAA<<";
    strncpy( pChar, replace.c_str(), replace.length() );

    ofstream fout( output.c_str(), ios_base::out );
    fout.write( content, size );
    fout.flush();
    fout.close();
}


int main( int argc, char const *argv[] ) {
    /* code */

    string output = "./output.txt";

    IOWholeFile( output );

    return 0;
}
