#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>


using namespace std;

void ReadToEofNTimes( fstream &fs, int N ) {
    string tmp;
    for ( int i = 0; i < N; ++i ) {
        fs.clear();
        fs.seekg( 0, ios_base::beg );
        for ( string tmp; getline( fs, tmp ); )
            cout << tmp << endl;
    }
}

int main( int argc, char const *argv[] ) {

    string file1 = "./file1.txt";
    fstream fs1( file1.c_str(), ios_base::in );
    ReadToEofNTimes( fs1, 3 );
    return 0;
}
