#include "KeyValueStorage.h"

using std::get;
using std::make_tuple;

/***********************************************
**************** public methods ****************
***********************************************/



KeyValueStorage::KeyValueStorage( string &input, string &output ) {

    // get input and output file, and open input file in read mode
    KeyValueStorage::inputFile = string( input );
    KeyValueStorage::outputFile = string( output );

    // ! check if db dir exists

    fstream fin( input.c_str(), std::ios_base::in );

    // init cmd buffer, todo queue, put buffer
    queue<string> qTodoBuf[DBNum];
    map<string, string> mPutBuffer[DBNum];
    KeyValueStorage::cmdBuffer = new string[MaxBufSize];

    // read commands to buffer until EOF
    int numOfCmds;
    while ( fin.eof() == false ) {

        // read cmds into buffer and get how many cmds read
        numOfCmds = ReadNCommands( fin, MaxBufSize, cmdBuffer );

        // parse all cmds in buffer
        int cmdIndex;
        CmdType cmdType;

        for ( int iCmd = 0; iCmd < numOfCmds; ++iCmd ) {

            // what is cmd type of this cmd
            std::tie( cmdType, cmdIndex ) = QuickParseCmd( cmdBuffer[iCmd] );

            // check cmd type
            switch ( cmdType ) {

                case PUT:
                    // push to corresponding todo queue
                    qTodoBuf[cmdIndex].push( cmdBuffer[iCmd] );
                    break;

                case GET:
                case SCAN:
                    // parse cmds in todo queue
                    for ( int iDB = 0; iDB < DBNum; ++iDB )
                        ParseTodoBuffer( qTodoBuf[iDB], mPutBuffer[iDB], iDB );

                    // update put commands in put buffer
                    for ( int iDB = 0; iDB < DBNum; ++iDB )


                        break;
            }
        }
    }
}
/************************************************
**************** private methods ****************
************************************************/

void KeyValueStorage::ParseTodoBuffer( queue<string> &qTodoBuf, map<string, string> &mPutBuf, int &dbID ) {
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
            mPutBuf.insert( key, value );

        // if already in put buffer -> update
        else
            iterValue->second = value;
    }
}

void UpdateDBFromPutBuffer( map<string, string> &mPutBuf, int &dbID ) {

    // where is my db
    string myDBPath = "./db/db0";
    myDBPath.back() = '0' + dbID;

    // db file stream
    fstream db, reader( myDBPath.c_str(), std::ios_base::in );

    // update all put cmds in buffer map
    for ( auto &pKV : mPutBuf ) {

        // check if this key in db
        reader.clear();
        reader.seekg( 0, std::ios_base::beg );
        bool lineEnd = FindKeyLineEndFrom( reader, pKV.first ).first;

        // if already in db, update
        if ( lineEnd != -1 ) {
            // open in out mode
            db.open( myDBPath.c_str(), std::ios_base::out );

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
    }

    // clear buffer map and close db stream
    mPutBuf.clear();
    db.close();
}

int KeyValueStorage::ReadNCommands( fstream &fin, int maxLines, string *cmdBuffer ) {
    // 一次讀最多 maxLines lines 到 buffer 中
    int count;
    for ( count = 0; count < maxLines && !fin.eof(); ++count )
        std::getline( fin, cmdBuffer[count] );

    // 檢查 eof, 如果 eof 就回傳 count 數量
    return fin.eof() ? count : maxLines;
}


tuple<CmdType, int> KeyValueStorage::QuickParseCmd( string &cmd ) {
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
    // DEBUG( "quick parse result : type %d, index %c, \ncmd :%s", type, lastChar, cmd.c_str() );
    return make_tuple( type, lastChar - '0' );
}


int KeyValueStorage::GetKeyFromCmd( string &cmd, string &key, int start, char delim ) {
    for ( int iCmd = start, iKey = 0;; ++iCmd, ++iKey ) {
        if ( cmd[iCmd] == delim )
            return ++iCmd;
        else
            key[iKey] = cmd[iCmd];
    }
}


void KeyValueStorage::GetValueFromCmd( string &cmd, string &value, int start ) {
    for ( int iCmd = start, iValue = 0; iValue < 128; ++iCmd, ++iValue )
        value[iValue] = cmd[iCmd];
}


tuple<string, string> KeyValueStorage::ParseCommandAs( CmdType type, string &cmd ) {

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
    return make_tuple( key, value );
}


pair<long, string> FindKeyLineEndFrom( fstream &db, string key ) {

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
            return pair( db.tellg(), tmp );
    }

    return pair( -1, "EMPTY" );
}