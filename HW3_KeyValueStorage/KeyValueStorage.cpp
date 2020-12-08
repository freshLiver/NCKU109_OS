#include "KeyValueStorage.h"

using std::get;
using std::make_tuple;



/***********************************************
**************** Initialization ****************
***********************************************/
string *KeyValueStorage::cmdBuffer;
fstream KeyValueStorage::fin, KeyValueStorage::fout;

map<string, long> *KeyValueStorage::lineEndCache;
set<string> *KeyValueStorage::usedPool;
set<string> *KeyValueStorage::victimPool;

static void Threading( queue<string> &qTodoPUTs, map<string, string> &mPutBuf, int id ) {
    KeyValueStorage::ParseTodoBuffer( qTodoPUTs, mPutBuf, id );
    KeyValueStorage::UpdateDBFromPutBuffer( mPutBuf, id );
}

void TouchFiles() {
    // check if db dir exists
    system( "mkdir ./db/" );
    for ( int dbID = 0; dbID < 10; ++dbID ) {
        string touchCmd = "touch ./db/db0";
        touchCmd[touchCmd.length() - 1] = '0' + dbID;
        system( touchCmd.c_str() );
    }
}

/***********************************************
**************** public methods ****************
***********************************************/


KeyValueStorage::KeyValueStorage( string &input, string &output ) {

    TouchFiles();

    // open input output file in read mode
    bool isFirstCmd = true;
    KeyValueStorage::fin.open( input.c_str(), std::ios_base::in );
    KeyValueStorage::fout.open( output.c_str(), std::ios_base::out );

    // init cmd buffer, todo queue, put buffer
    queue<string> qTodoBuf[DBNum];
    map<string, string> mPutBuf[DBNum];

    // init cache and pools
    KeyValueStorage::lineEndCache = new map<string, long>[DBNum];
    KeyValueStorage::victimPool = new set<string>[DBNum];
    KeyValueStorage::usedPool = new set<string>[DBNum];

    // read commands to buffer until EOF
    int numOfCmds;
    int progress = 0;

    // read until eof
    while ( KeyValueStorage::fin.eof() == false ) {
        int cmdIndex;
        CmdType cmdType;
        string cmd;

        // sequential read command
        for ( int iCmd = 0; iCmd < MaxBufSize && getline( KeyValueStorage::fin, cmd ); ++iCmd ) {
            if ( progress % 10000 == 0 )
                printf( "now : %d\n", progress );
            progress++;

            // read a cmd and quick parse it immediately
            std::tie( cmdType, cmdIndex ) = KeyValueStorage::QuickParseCmd( cmd );

            // check cmd type
            switch ( cmdType ) {

                case PUT:
                    // push to corresponding todo queue
                    qTodoBuf[cmdIndex].push( cmd );
                    break;

                case GET:
                case SCAN:
                    // start pos and num of gets
                    LL begin, numOfGets = 1;

                    // parse this command into key, value or key1, key2
                    auto parseResult = ParseCommandAs( cmdType, cmd );

                    // convert scan and get to begin and numOfGets
                    begin = stoll( parseResult.first );
                    if ( cmdType == SCAN )
                        numOfGets = stoll( parseResult.second ) - begin + 1;

                    // parse cmds in todo queue
                    thread ths[10];
                    for ( int id = 0; id < DBNum; ++id )
                        ths[id] = thread( Threading, std::ref( qTodoBuf[id] ), std::ref( mPutBuf[id] ), id );

                    for ( int thID = 0; thID < DBNum; ++thID )
                        ths[thID].join();

                    // do all get cmds and output result to output file
                    for ( LL count = 0; count < numOfGets; ++count, ++begin ) {
                        string value = GetValueByKey( std::to_string( begin ), ( cmdIndex++ ) % 10 );
                        if ( isFirstCmd == true ) {
                            isFirstCmd = false;
                            KeyValueStorage::fout << value.c_str();
                        }
                        else
                            KeyValueStorage::fout << "\n" << value.c_str();
                    }
                    break;
            }
        }
    }
    KeyValueStorage::fin.close();
    KeyValueStorage::fout.flush();
    KeyValueStorage::fout.close();
}


/************************************************
**************** private methods ****************
************************************************/

pair<CmdType, int> KeyValueStorage::QuickParseCmd( string &cmd ) {
    int keyPos;
    CmdType type;
    // get key start position and cmd type
    switch ( cmd[0] ) {
        case 'P':
            keyPos = 4;
            type = PUT;
            break;

        case 'G':
            keyPos = 4;
            type = GET;
            break;

        case 'S':
            keyPos = 5;
            type = SCAN;
            break;
    }

    // get char in order until space(key end)
    char lastChar;
    for ( int iCmd = keyPos;; ++iCmd )
        if ( cmd[iCmd] == ' ' || cmd[iCmd] == '\0' ) {
            lastChar = cmd[iCmd - 1];
            break;
        }

    // 回傳型別以及最後一個數字
    return pair<CmdType, int>( type, lastChar - '0' );
}


pair<string, string> KeyValueStorage::ParseCommandAs( CmdType type, string &cmd ) {

    string key( 20, '\0' ), value( 129, '\0' );
    int end;
    switch ( type ) {
        // * GET  : GET key[<20]
        case PUT:
            end = GetKeyFromCmd( cmd, key, 4, ' ' );
            GetValueFromCmd( cmd, value, end );
            break;

        // * PUT  : PUT key[<20] value[128]
        case GET:
            GetKeyFromCmd( cmd, key, 4, '\0' );
            break;

        // * SCAN : SCAN key1[<20] key2[<20]
        case SCAN:
            end = GetKeyFromCmd( cmd, key, 5, ' ' );
            GetKeyFromCmd( cmd, value, end, '\0' );
            break;
    }
    return pair<string, string>( key, value );
}


void KeyValueStorage::ParseTodoBuffer( queue<string> &qTodoBuf, map<string, string> &mPutBuf, int dbID ) {
    string cmd, key, value;
    while ( qTodoBuf.empty() == false ) {

        // get and pop front cmd
        cmd = qTodoBuf.front();
        qTodoBuf.pop();

        // get key and value, and push to put buffer
        std::tie( key, value ) = KeyValueStorage::ParseCommandAs( PUT, cmd );


        auto iterValue = mPutBuf.find( key );
        // if not in buffer -> insert
        if ( iterValue == mPutBuf.end() )
            mPutBuf.insert( pair<string, string>( key, value ) );

        // if already in put buffer -> update
        else
            iterValue->second = value;
    }
}


pair<long, string> KeyValueStorage::FindKeyLineEndFrom( fstream &db, string key ) {

    // 清除 eof flag 並回到檔案開頭
    db.clear();
    db.seekg( 0, std::ios::beg );

    // 依據查詢各行
    for ( string tmp; getline( db, tmp ); ) {

        // 比對該行 key
        bool sameKey = true;
        for ( int i = 0; ( key[i] != '\0' ) && sameKey; ++i )
            sameKey = ( key[i] == tmp[i] );

        // 若 key 完全相同就回傳結果
        if ( sameKey )
            return pair<long, string>( db.tellg(), tmp );
    }

    return pair<long, string>( -1, "EMPTY" );
}


void KeyValueStorage::UpdateDBFromPutBuffer( map<string, string> &mPutBuf, int dbID ) {

    // where is my db
    string myDBPath = "./db/db0";
    myDBPath[myDBPath.length() - 1] = '0' + dbID;

    // db file stream
    fstream db, reader( myDBPath.c_str(), std::ios_base::in );


    // update all put cmds in buffer map
    for ( auto &pKV : mPutBuf ) {

        // TODO check cache 

        // check if this key in db
        reader.clear();
        reader.seekg( 0, std::ios_base::beg );
        long lineEnd = KeyValueStorage::FindKeyLineEndFrom( reader, pKV.first ).first;

        // if already in db, update
        if ( lineEnd != -1 ) {
            // open in out mode
            db.open( myDBPath.c_str() );

            // move write pointer
            db.seekp( lineEnd - 129, std::ios_base::beg );

            // write to file
            db << pKV.second.c_str();
        }

        // if not in db, append
        else {
            // open in app mode
            db.open( myDBPath.c_str(), std::ios_base::app );

            // appen "key value\n"
            db << pKV.first.c_str() << " " << pKV.second.c_str() << std::endl;
        }

        // ! dont forget flush
        db.flush();
        db.clear();
        db.close();
    }

    // clear buffer map and close db stream
    mPutBuf.clear();
    db.close();
}



string KeyValueStorage::GetValueByKey( string key, int dbID ) {

    // where is my db
    string myDBPath = "./db/db0";
    myDBPath[myDBPath.length() - 1] = '0' + dbID;

    // 開啟並從自己的 db 中搜尋這個 key
    fstream reader( myDBPath.c_str(), std::ios_base::in );
    auto result = KeyValueStorage::FindKeyLineEndFrom( reader, key );
    reader.clear();
    reader.close();

    // lineEnd == -1 代表找不到 key，回傳 "EMPTY"
    if ( result.first == -1 )
        return "EMPTY";

    // 如果有找到 key 就抓 keyline(res.second) 最後 128 個 char(不含\n)
    int len = result.second.length();
    return result.second.substr( len - 128, 128 );
}
