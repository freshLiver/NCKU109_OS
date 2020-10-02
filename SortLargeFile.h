#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <string>

using std::queue;
using std::string;

typedef long long LL;

#define APPEND_MODE 2
#define SPLIT_MODE 1
#define MERGE_MODE 0
#define NOT_DEBUG_MODE false

#define LOG(...) printf(__VA_ARGS__)
#define TimeLog(x) printf(">> cost time : %d\n",x)

class SortLargeFile {
  private:
    LL maxBuffSize, nowBuffSize;
    int merged_counter, *buffer;
    const char *srcData, *destDir;
    queue< string > filePool;

  public:
    // contructor
    SortLargeFile ( const char *largeFile, const char *destDir );


    // methods
    void split2sorteds ( );
    void merge2sorteds ( string src1, string src2 );
    void bufout ( short mode );
    FILE *GetOutputFile ( short mode, char *filename );


    // static methods
    static int *FixedIntArray ( int GBs, LL &maxSize ) {
        maxSize = ( LL ) GBs * 1024 * 1024 * 1024 / sizeof ( int );
        new int[maxSize];
    }
};
