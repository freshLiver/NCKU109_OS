#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

using std::cout;
using std::endl;

using std::ifstream;
using std::ofstream;
using std::queue;
using std::string;


int main ( int argc, char const *argv[] ) {

    // input file from argv[1]
    ifstream fin ( argv[1], std::ios::in );

    // read all and push to queue
    queue< string > contents;
    for ( string tmp; std::getline ( fin, tmp ); )
        contents.push ( tmp );
    fin.close ( );

    // output file to argv[2]
    time_t start = time ( NULL );
    ofstream fout ( argv[2], std::ios::out );
    // FILE *fout = fopen ( argv[2], "w" );
    while ( !contents.empty ( ) ) {
        // fprintf ( fout, "%s\n", contents.front ( ).c_str ( ) );
        fout << contents.front ( ) << endl;
        contents.pop ( );
    }
    fout.close ( );
    // fflush ( fout );
    // fclose ( fout );
    cout << " fout << cost : " << time ( NULL ) - start << endl;

    return 0;
}
