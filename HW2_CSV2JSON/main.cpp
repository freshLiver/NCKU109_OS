#include "CVS2JSON.h"
#include <fstream>

using std::ifstream;

static int CStr2Int32 ( const char *str ) {
    int result;



    return result;
}

int main ( int argc, char const *argv[] ) {
    // handle args and open file
    int threads = ( argv[2] == NULL ) ? 1 : CStr2Int32 ( argv[2] );
    ifstream input ( argv[1], std::ios::in );

    // get every line
    int count = 0;        // TODO: MAX LINES ?
    for ( string tmp; std::getline ( input, tmp ); ++count )
        CVS2JSON::datas.push ( pair< int, string > ( count, tmp ) );

    // build class
    CVS2JSON cvs2json ( count, threads );

    // 

    
    return 0;
}
