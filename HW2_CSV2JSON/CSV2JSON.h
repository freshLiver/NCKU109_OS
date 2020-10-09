#include <cstdio>
#include <cstdlib>
#include <future>
#include <queue>
#include <string>
#include <thread>


using std::pair;
using std::queue;
using std::string;
using std::thread;
using std::async;
using std::future;

#define SINGLE_MODE 0

// debug macros
#define DEBUG( format, ... ) printf ( "DEBUG >> " format "\n", __VA_ARGS__ )
#define COST_TIME( str ) DEBUG ( "%s cost : %lu secs.", str, time ( NULL ) - start )
#define RESET_TIME( ) start = time ( NULL )

// result cells
typedef struct VALUES {
    int values[20];

    VALUES ( ) {}
    VALUES ( int *value ) {
        for ( int i = 0; i < 20; ++i )
            values[i] = value[i];
    }

} Values, *pValues;


// main class
class CSV2JSON {
  public:
    static int lines;
    static short threads;

    static pValues results;
    static queue< pair< int, string > > datas;

    // CTOR
    CSV2JSON ( int totalLines, short threads );

    // parse line and store values
    static void ParseLineStoreValues ( );

  private:
    // async get data for thread safety
    static pair< int, string > AsyncPopQueue ( ) {
        // get front data and pop it
        pair< int, string > data = CSV2JSON::datas.front ( );
        CSV2JSON::datas.pop ( );

        // return front data
        return data;
    }
};
