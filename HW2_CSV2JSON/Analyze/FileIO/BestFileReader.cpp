// TODO: COMPARE fscanf, getline, fstream >>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

using std::cout;
using std::endl;
using std::queue;
using std::string;
using std::fstream;
using std::ifstream;

#define COST_TIME( s, c ) printf ( "%s[%lu] cost : %lds\n", s, c, time ( NULL ) - start )

int main ( int argc, char const *argv[] ) {

    time_t start;
    // result tmp queue
    queue< char * > *fsfRes = new queue< char * > ( );
    queue< string > *ifsRes = new queue< string > ( );
    queue< string > *gtlRes = new queue< string > ( );

    // file read tmp
    int count;
    char tmpChars[250];
    string tmpStr;


    // fscanf until EOF
    FILE *target = fopen ( argv[1], "r" );
    start = time ( NULL );
    for ( count = 0; EOF != fscanf ( target, "%s", tmpChars ); ++count )
        fsfRes->push ( tmpChars );
    fclose ( target );
    COST_TIME ( "fscanf", fsfRes->size ( ) );
    free ( fsfRes );

    // ifstream until EOF
    ifstream ifs ( argv[1], std::ios::in );
    start = time ( NULL );
    for ( count = 0; ifs >> tmpStr; ++count )
        ifsRes->push ( tmpStr );
    ifs.close ( );
    COST_TIME ( "ifstream", ifsRes->size ( ) );
    free ( ifsRes );

    // getline until EOF
    start = time ( NULL );
    ifstream gtl ( argv[1], std::ios::in );
    for ( count = 0; std::getline ( gtl, tmpStr ); ++count )
        gtlRes->push ( tmpStr );
    gtl.close ( );
    COST_TIME ( "getline", gtlRes->size ( ) );
    free ( gtlRes );

    // end
    printf("---------------------------------------\n");
    return 0;
}
