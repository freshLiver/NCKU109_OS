#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <queue>
#include <string>

using std::string;
using std::ifstream;
using std::istream;
using std::ios;
using std::pair;
using std::queue;

int main ( int argc, char const *argv[] ) {

    ifstream input ( argv[1], ios::in );
    queue< pair< int, string > > datas;

    int count = 0;
    for ( string tmp; std::getline ( input, tmp ); ++count ) {
        datas.push ( pair <int,string>( count, tmp ) );
    }
    printf ( "total : %d lines\n", count );
    return 0;
}
