#include "macros.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <queue>
#include <string>
#include <thread>


using std::pair;
using std::queue;
using std::string;
using std::thread;

#define SINGLE_THREAD 1
#define Numpl 20        // numbers per line



// main class
class CSV2JSON {
public:
    static uint lines;
    static uint workers;

    static string *cells;
    static queue< pair< uint, string > > *queues;

    // CTOR
    CSV2JSON ( string input, string output, uint workers );



private:
    //
    // Read csv file and push to target queues, return total lines
    //
    static uint ReadCSV ( string input, uint n ) {
        // open file
        std::ifstream fin ( input.c_str ( ), std::ios::in );

        // read until EOF
        uint count = 0;
        for ( string tmp; std::getline ( fin, tmp ); ++count ) {
            // push to which queue ?
            uint target = ( n == 1 ) ? 0 : count % n;
            CSV2JSON::queues[target].push ( pair< uint, string > ( count, tmp ) );
        }

        // close file and return total lines
        fin.close ( );
        return count;
    }
    //
    // parse line and store values
    //
    static void ThreadingParseDatas ( uint worker_id );
    //
    // split a string has 20 numbers that split by '|' into string[]
    //
    static void Split2List ( string raw, string *list );
    //
    // turn the result of CSV2JSON::Split2List into json format that is defined in "macros.h"
    //
    static string List2Cell ( string *list );
};
