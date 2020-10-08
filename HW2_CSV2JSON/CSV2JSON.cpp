#include "CSV2JSON.h"

CSV2JSON::CSV2JSON ( int totalLines, short threads ) {
    // set total cvs lines and multi-thread mode
    CSV2JSON::lines = totalLines;
    CSV2JSON::threads = threads;
    // create $(lines) cells to store result
    CSV2JSON::kvs = new KeyValues[totalLines];

}


