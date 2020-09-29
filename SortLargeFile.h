#include <bits/stdc++.h>
#include <cstdio>
#include <cstdlib>
#include <queue>

using std::queue;

typedef long long LL;

#define APPEND_MODE 2
#define SPLIT_MODE 1
#define MERGE_MODE 0

class SortLargeFile {
  private:
    LL maxBuffSize, nowBuffSize;
    int merged_counter, *buffer;
    const char *srcData, *destDir;
    queue< char * > filePool;

  public:
    // contructor
    SortLargeFile ( const char *largeFile,const char *destDir );


    // methods
    void split2sorteds ( );
    void merge2sorteds ( char *src1, char *src2 );
    void bufout ( bool mode );
    FILE *GetOutputFile ( int mode, char *filename );


    // static methods
    static int *FixedIntArray ( int GBs, LL &maxSize ) {
        maxSize = ( LL ) GBs * 1024 * 1024 * 1024 / sizeof ( int );
        new int[maxSize];
    }
};
