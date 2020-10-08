#include "CVS2JSON.h"

CVS2JSON::CVS2JSON ( int totalLines, short threads ) {
    // set total cvs lines and multi-thread mode
    CVS2JSON::lines = totalLines;
    CVS2JSON::threads = threads;
    // create $(lines) cells to store result
    CVS2JSON::kvs = new KeyValues[totalLines];

}


