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
#define BUF_SIZE__MBs 1024


class SortLargeFile {
  private:
    LL maxBuffSize, nowBuffSize;
    int merged_counter, *buffer;
    const char *srcData, *destDir;
    queue< string > filePool;

  public:
    string outputFile;
    // contructor
    SortLargeFile ( const char *largeFile, const char *destDir );


    // methods
    void split2sorteds ( );
    void merge2sorteds ( string src1, string src2 );
    void bufout ( short mode );
    FILE *GetOutputFile ( short mode, char *filename );


    // static methods
    static int *FixedIntArray ( int MBs, LL &maxSize ) {
        maxSize = ( LL ) MBs * 1024 * 1024 / sizeof ( int );
        return new int[maxSize];
    }
};
