#include "KeyValueStorage.h"

using std::get;
using std::make_tuple;

/***********************************************
**************** public methods ****************
***********************************************/

pair<long, string> FindKeyLineEndFrom( fstream &db, string key );
void UpdateDBFromPutBuffer( map<string, string> &mPutBuf, int &dbID );
string GetValueByKey( string key, int &dbID );

string KeyValueStorage::inputFile, KeyValueStorage::outputFile;
string *KeyValueStorage::cmdBuffer;


KeyValueStorage::KeyValueStorage( string &input, string &output ) {

    // get input and output file
    KeyValueStorage::inputFile = string( input );
    KeyValueStorage::outputFile = string( output );

    // check if db dir exists
    system( "mkdir ./db/" );
    for ( int dbID = 0; dbID < 10; ++dbID ) {
        string touchCmd = "touch ./db/db0";
        touchCmd[touchCmd.length() - 1] = '0' + dbID;
        system( touchCmd.c_str() );
    }

    // open input output file in read mode
    bool isFirstCmd = true;
    fstream fin( input.c_str(), std::ios_base::in );
    fstream fout( output.c_str(), std::ios_base::out );

    // init cmd buffer, todo queue, put buffer
    queue<string> qTodoBuf[DBNum];
    map<string, string> mPutBuf[DBNum];
    KeyValueStorage::cmdBuffer = new string[MaxBufSize];

    // read commands to buffer until EOF
    int numOfCmds;
    while ( fin.eof() == false ) {

        // read cmds into buffer and get how many cmds read
        numOfCmds = ReadNCommands( fin, MaxBufSize, cmdBuffer );

        // parse all cmds in buffer
        int cmdIndex;
        CmdType cmdType;
        string currentCmd;

        for ( int iCmd = 0; iCmd < numOfCmds; ++iCmd ) {
            currentCmd = cmdBuffer[iCmd];

            // what is cmd type of this cmd
            std::tie( cmdType, cmdIndex ) = QuickParseCmd( currentCmd );

            // check cmd type
            switch ( cmdType ) {

                case PUT:
                    // push to corresponding todo queue
                    qTodoBuf[cmdIndex].push( currentCmd );
                    break;

                case GET:
                case SCAN:
                    // start pos and num of gets
                    LL begin, numOfGets = 1;

                    // parse this command into key, value or key1, key2
                    auto parseResult = ParseCommandAs( cmdType, currentCmd );

                    // convert scan and get to begin and numOfGets
                    begin = stoll( parseResult.first );
                    if ( cmdType == SCAN )
                        numOfGets = stoll( parseResult.second ) - begin + 1;

                    // parse cmds in todo queue
                    for ( int dbID = 0; dbID < DBNum; ++dbID )
                        ParseTodoBuffer( qTodoBuf[dbID], mPutBuf[dbID], dbID );

                    // update put commands in put buffer
                    for ( int dbID = 0; dbID < DBNum; ++dbID )
                        UpdateDBFromPutBuffer( mPutBuf[dbID], dbID );

                    // do all get cmds and output result to output file
                    for ( LL count = 0; count < numOfGets; ++count ) {
                        string value = GetValueByKey( std::to_string( begin + count ), cmdIndex );
                        if ( isFirstCmd == true ) {
                            isFirstCmd = false;
                            fout << value.c_str();
                        }
                        else
                            fout << "\n" << value.c_str();
                    }
                    break;
            }
        }
    }

    fin.close();
    fout.flush();
    fout.close();
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
            mPutBuf.insert( pair<string, string>( key, value ) );

        // if already in put buffer -> update
        else
            iterValue->second = value;
    }
}

int KeyValueStorage::ReadNCommands( fstream &fin, int maxLines, string *cmdBuffer ) {
    // 一次讀最多 maxLines lines 到 buffer 中
    int count;
    for ( count = 0; count < maxLines && !fin.eof(); ++count )
        std::getline( fin, cmdBuffer[count] );

    // 檢查 eof, 如果 eof 就回傳 count 數量
    return fin.eof() ? count : maxLines;
}


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
    // DEBUG( "quick parse result : type %d, index %c, \ncmd :%s", type, lastChar, cmd.c_str() );
    return pair<CmdType, int>( type, lastChar - '0' );
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


// not members

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
            return pair<long, string>( db.tellg(), tmp );
    }

    return pair<long, string>( -1, "EMPTY" );
}


void UpdateDBFromPutBuffer( map<string, string> &mPutBuf, int &dbID ) {

    // where is my db
    string myDBPath = "./db/db0";
    myDBPath[myDBPath.length() - 1] = '0' + dbID;

    // db file stream
    fstream db, reader( myDBPath.c_str(), std::ios_base::in );


    // update all put cmds in buffer map
    for ( auto &pKV : mPutBuf ) {

        // check if this key in db
        reader.clear();
        reader.seekg( 0, std::ios_base::beg );
        long lineEnd = FindKeyLineEndFrom( reader, pKV.first ).first;

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
        if ( db.badbit || db.failbit )
            printf( "flush fail" );
        db.close();
    }

    // clear buffer map and close db stream
    mPutBuf.clear();
    db.close();
}

string GetValueByKey( string key, int &dbID ) {

    // where is my db
    string myDBPath = "./db/db0";
    myDBPath[myDBPath.length() - 1] = '0' + dbID;

    // 開啟並從自己的 db 中搜尋這個 key
    fstream reader( myDBPath.c_str(), std::ios_base::in );
    auto result = FindKeyLineEndFrom( reader, key );
    reader.close();

    // lineEnd == -1 代表找不到 key，回傳 "EMPTY"
    if ( result.first == -1 )
        return "EMPTY";

    // 如果有找到 key 就抓 keyline(res.second) 最後 128 個 char(不含\n)
    char value[129];
    for ( int iValue = 0, iCmd = ( result.second.length() - 128 ); iValue < 128; ++iValue, ++iCmd )
        value[iValue] = result.second[iCmd];
    value[128] = '\0';
    return string( value );
}
