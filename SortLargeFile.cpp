#include "SortLargeFile.h"

/// CTOR
SortLargeFile::SortLargeFile ( const char *src, const char *dest ) {
    // src and dest info
    this->srcData = src;
    this->destDir = dest;

    // init datas and buffer
    this->nowBuffSize = 0;
    this->merged_counter = 0;
    this->buffer = SortLargeFile::FixedIntArray ( 1, maxBuffSize );
    printf ( ">>>> init finished. buffer[%lld] \n", this->maxBuffSize );

    // split into sorted small files and add to queue
    this->split2sorteds ( );

    // merge these sorted small files
    while ( this->filePool.size ( ) > 1 ) {
        // get and pop two files
        string src1 = this->filePool.front ( ).c_str ( );
        this->filePool.pop ( );
        string src2 = this->filePool.front ( ).c_str ( );
        this->filePool.pop ( );

        // merge the two file
        try {
            this->merge2sorteds ( src1, src2 );
        }
        catch ( const std::exception &e ) {
            std::cerr << e.what ( ) << '\n';
        }

        // ++counter
        ++this->merged_counter;
    }

    // the remains file is the result file
    printf ( ">>>>> ALL FINISHED, Result at : %s <<<<<\n", this->filePool.back ( ).c_str ( ) );
}



/// determine output filename by mode
FILE *SortLargeFile::GetOutputFile ( short mode, char *filename ) {
    printf ( ">>>> filename src addr is %p\n", &filename );

    // determine output file name by mode
    if ( mode == SPLIT_MODE )
        sprintf ( filename, "%s/outputs/part%ld", this->destDir, this->filePool.size ( ) );
    else
        sprintf ( filename, "%s/outputs/merged%d", this->destDir, this->merged_counter );

    // w mode if SPLIT_MODE
    return fopen ( filename, ( ( ( mode == SPLIT_MODE ) && NOT_DEBUG_MODE ) ? "w" : "a" ) );
}



/// output current buffer contents to file (split or merge)
void SortLargeFile::bufout ( short mode ) {
    // open file
    char filename[100];
    FILE *output = this->GetOutputFile ( mode, filename );

    // split mode should sort file and add to file pool
    if ( mode == SPLIT_MODE ) {
        if ( NOT_DEBUG_MODE )        // TODO GUST FOR TEST
            std::sort ( this->buffer, this->buffer + this->nowBuffSize );
        string char2string ( filename );
        this->filePool.push ( char2string );
    }
    printf ( ">>>> output to : %s<<< \n", filename );

    // output buffer
    for ( LL count = 0; count < this->nowBuffSize; ++count )
        if ( NOT_DEBUG_MODE )
            fprintf ( output, "%d\n", this->buffer[count] );

        // TODO MERGE + DEBUG MODE ONLY
        else if ( ( mode == MERGE_MODE ) && ( count % 10000000 == 0 ) )
            printf ( ">> buffer out, buf[%.10lld] = %d\n", count, this->buffer[count] );

    printf ( ">>>> output finished. pool size : %ld\n", this->filePool.size ( ) );

    // close file and reset current buff size
    fclose ( output );
    this->nowBuffSize = 0;
}



/// split into "sorted" small files
void SortLargeFile::split2sorteds ( ) {
    // read a data with many many INT32 nums
    FILE *dataSet = fopen ( this->srcData, "r" );

    // split whole file into several small "Sorted "files
    for ( int value; EOF != fscanf ( dataSet, "%d", &value ); ++this->nowBuffSize ) {
        // if buffer is full, should output buffer
        if ( this->nowBuffSize >= this->maxBuffSize )
            this->bufout ( SPLIT_MODE );

        // buffer not full or after buffer out, add next value
        this->buffer[this->nowBuffSize] = value;
    }
    // maybe buffer not full but EOF, also need to output buffer
    this->bufout ( SPLIT_MODE );

    // reset current buffer
    this->nowBuffSize = 0;
}



/// merge 2 sorted small files
void SortLargeFile::merge2sorteds ( string src1, string src2 ) {
    // read from 2 sorted file
    FILE *part1 = fopen ( src1.c_str ( ), "r" );
    FILE *part2 = fopen ( src2.c_str ( ), "r" );
    printf ( ">>>> merge 2 files : \n\tsrc1 : %s\n\tsrc2 : %s\n", src1.c_str ( ), src2.c_str ( ) );

    // read first line of two file
    int value1, value2;
    short res1 = fscanf ( part1, "%d", &value1 );
    short res2 = fscanf ( part2, "%d", &value2 );

    // compare two file until at least one EOF
    for ( ; ( EOF != res1 && EOF != res2 ); ++this->nowBuffSize ) {
        // if buffer full
        if ( this->nowBuffSize >= this->maxBuffSize )
            this->bufout ( MERGE_MODE );

        // buffer not full or after buffer out,
        if ( this->nowBuffSize % 10000000 == 0 )
            printf ( ">> buf[%.10lld] = %d\n", this->nowBuffSize, this->buffer[this->nowBuffSize] );

        this->buffer[this->nowBuffSize] = ( value1 < value2 ) ? value1 : value2;

        // smaller one move to next value
        if ( value1 < value2 )
            res1 = fscanf ( part1, "%d", &value1 );
        else
            res2 = fscanf ( part2, "%d", &value2 );
    }

    // clear buffer and reset current size
    this->bufout ( MERGE_MODE );
    this->nowBuffSize = 0;

    printf ( ">>>> One File is EOF, append remain values directly \n" );

    // append remain files directly
    if ( res1 != res2 ) {        // TODO SOME THING WRONG
        // which has remain nums
        FILE *remain = ( EOF != res1 ) ? part1 : part2;
        char filename[100];
        FILE *target = this->GetOutputFile ( APPEND_MODE, filename );

        // determine
        int count = 0;
        for ( int value; EOF != fscanf ( remain, "%d", &value ); )
            if ( NOT_DEBUG_MODE )
                fprintf ( target, "%d\n", value );
            else if ( count++ % 100000 == 0 )
                printf ( ">> append %.9d directly. \n", value );
    }

    printf ( ">>>> MERGE FINISHED : \n\t%s\n\t%s\n", src1.c_str ( ), src2.c_str ( ) );
}