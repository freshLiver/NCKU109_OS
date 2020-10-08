#include <cstdio>
#include <cstdlib>
#include <queue>
#include <string>
#include <thread>

using std::pair;
using std::queue;
using std::string;
using std::thread;

#define SINGLE_MODE 0

// result cells
typedef struct KVS {
    int key;
    int values[20];

    KVS ( ) {}
    KVS ( int line, int *value ) : key ( line ) {
        for ( int i = 0; i < 20; ++i )
            values[i] = value[i];
    }

} KeyValues, *pKeyValues;


// main class
class CVS2JSON {
  public:
    static int lines;
    static short threads;
    static pKeyValues kvs;
    static queue< pair< int, string > > datas;

    CVS2JSON ( int totalLines, short threads );
    static void ParseRawDatas ( );

  private:
    static void SortResult ( );
};