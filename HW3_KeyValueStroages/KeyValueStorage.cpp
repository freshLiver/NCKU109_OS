#include "KeyValueStorage.h"
#include <unistd.h>

KeyValueStorage::KeyValueStorage( string input, string output ) {
    // get input and output file name
    KeyValueStorage::inputFile = input;
    KeyValueStorage::outputFile = output;

    // init every thread and their queue
    for ( int i = 0; i < 10; ++i ) {
    }
}

void KeyValueStorage::ReadCommands() {
    // open input file
    std::ifstream input( inputFile.c_str(), std::ios::in );

    // end parsing thread with 2 situations : EOF, Line MAX_READ
    bool eofStop = false, maxReadPause = false;

    // ! one thread push to queue, the other thread push to worker queue until GET
    queue<string> cmdQueue;
    thread thParse( ParseOnReading, &cmdQueue, &eofStop, &maxReadPause );


    while ( eofStop ) {
        // read N commands from input file at once
        int count = 0;
        queue<string> cmdQueue;
        for ( string tmp; count < MAX_READ && eofStop; ++count ) {
            // read a line and push to cmdQueue
            std::getline( input, tmp );
            // push to command vector
            cmdQueue.push( tmp );
        }
    }
}
//
//
//
void KeyValueStorage::ParseOnReading( queue<string> &cmdQueue, bool &stop, bool &pause ) {
    while ( stop || pause ) {
        while ( cmdQueue.size() < 0 ) {
            // pop first element from queue
            string cmd = cmdQueue.front();
            cmdQueue.pop();

            // parse this cmd and get cmd info
            auto res = KeyValueStorage::ParseCommand( cmd );
            CmdType type = get<CType>( res );
            string key = get<1>( res ), value_key2 = get<2>( res );

            // if PUT, just push to worker queue
            if ( type == PUT ) {
                int worker = key.back() - '0';
                KeyValueStorage::workerCmds[worker].push( make_tuple( key, value_key2 ) );
            }
            // if SCAN or GET,
            else {
            }
        }
    }
}