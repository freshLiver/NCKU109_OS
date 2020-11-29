#if !defined( KEYVALUESTORAGE_H )
#define KEYVALUESTORAGE_H

#include "macros.h"
#include <cstdio>
#include <exception>
#include <fstream>
#include <queue>
#include <string>
#include <thread>
#include <tuple>

using std::queue;
using std::string;
using std::thread;
using std::tuple;
using std::fstream;

class KeyValueStorage {

public:
private:
    static string inputFile, outputFile;
    static thread dbWorkers[10];
    static fstream dbFiles[10];
    static queue<tuple<string, string>> workerCmds[10];


public:
    KeyValueStorage( string input, string output );


    static void ReadNCommands( fstream &input, int N, string cmdBuffer[] );


    static tuple<CmdType, int> QuickParseCmd( string &cmd ) {}


private:
    static int GetKeyFromCmd( string &cmd, string &key, int start, char delim );


    static void GetValueFromCmd( string &cmd, string &value, int start );


    static tuple<string, string> ParseCommandAs( CmdType type, string cmd );


    static tuple<CmdType, string, string> ParseCommand( string cmd );
};

#endif        // KEYVALUESTORAGE_H