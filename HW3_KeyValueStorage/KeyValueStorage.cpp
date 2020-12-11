#include "KeyValueStorage.h"

using std::get;
using std::ref;
using std::make_pair;
using std::make_tuple;


/***********************************************
**************** Initialization ****************
***********************************************/
string *KeyValueStorage::cmdBuffer;
fstream KeyValueStorage::fin, KeyValueStorage::fout;

map<string, long> *KeyValueStorage::lineEndCache;
set<string> *KeyValueStorage::usedPool;
set<string> *KeyValueStorage::victimPool;

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

    srand( time( NULL ) );

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
        bool isPUT = true;
        int iCmd = 0;
        for ( ; isPUT && iCmd < MaxBufSize && getline( KeyValueStorage::fin, cmd ); ++iCmd ) {
            if ( progress % 10000 == 0 )
                printf( "now : %d\n", progress );
            progress++;

            // read a cmd and quick parse it immediately
            std::tie( cmdType, cmdIndex ) = KeyValueStorage::QuickParseCmd( cmd );

            // check cmd type
            if ( cmdType == PUT )
                // push to corresponding todo queue
                qTodoBuf[cmdIndex].push( cmd );
            else
                isPUT = false;
        }

        // 根據 iCmd 大小（連續 PUT 數量）決定要用哪種方式處理累積的 PUT 指令
        if ( iCmd >= ThreadingThreshold ) {
            thread ths[10];
            for ( int id = 0; id < DBNum; ++id )
                ths[id] = thread( KeyValueStorage::ParseTodoBuffer, ref( qTodoBuf[id] ), ref( mPutBuf[id] ), id );

            for ( int id = 0; id < DBNum; ++id )
                ths[id].join();
        }
        // 連續 PUT 太少，不如 sequential
        else {
            for ( int id = 0; id < DBNum; ++id )
                KeyValueStorage::ParseTodoBuffer( qTodoBuf[id], mPutBuf[id], id );
        }

        for ( int id = 0; id < DBNum; ++id )
            KeyValueStorage::UpdateDBFromPutBuffer( mPutBuf[id], id );


        // ! 檢查 cmd type (檢查是 buffer 滿了跳出還是遇到 GET, SCAN)
        if ( isPUT == false ) {

            // 把 SCAN, GET generalize 成 begin(key & index) + numOfGets 的形式
            auto parseResult = ParseCommandAs( cmdType, cmd );
            LL numOfGets = 1, begin = stoll( parseResult.first );

            if ( cmdType == SCAN )
                numOfGets = stoll( parseResult.second ) - begin + 1;

            // 從 begin 開始進行 GET，每次都需要 index++%10 更換 DB
            for ( LL count = 0; count < numOfGets; ++count, ++begin ) {
                string value = GetValueByKey( std::to_string( begin ), ( cmdIndex++ ) % 10 );
                if ( isFirstCmd == true ) {
                    isFirstCmd = false;
                    KeyValueStorage::fout << value.c_str();
                }
                else
                    KeyValueStorage::fout << "\n" << value.c_str();
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


pair<long, string> KeyValueStorage::FindKeyLineEndFrom( fstream &db, string key, int dbID ) {

    // 清除 eof flag 並回到檔案開頭
    db.clear();

    // 檢查 cache
    auto cacheResult = KeyValueStorage::lineEndCache[dbID].find( key );

    // if find in cache, set line end
    if ( cacheResult != KeyValueStorage::lineEndCache[dbID].end() ) {

        // 把 key 從 victim 移動到 used pool
        auto itVictim = KeyValueStorage::victimPool[dbID].find( key );
        if ( itVictim != KeyValueStorage::victimPool[dbID].end() ) {
            KeyValueStorage::victimPool[dbID].erase( key );
            KeyValueStorage::usedPool[dbID].insert( key );
        }

        // move read pointer to value[0]
        db.seekg( cacheResult->second - 129 );

        // read 128 values
        char value[129];
        db.read( value, 128 );
        value[128] = '\0';

        // return line end and value
        return make_pair( cacheResult->second, string( value ) );
    }

    // if not in cache
    else {
        db.seekg( 0, std::ios::beg );

        // 依據查詢各行
        for ( string tmp; getline( db, tmp ); ) {

            // 比對該行 key
            bool sameKey = true;
            for ( int i = 0; ( key[i] != '\0' ) && sameKey; ++i )
                sameKey = ( key[i] == tmp[i] );

            // 若 key 完全相同，代表在 db 中
            if ( sameKey ) {
                long lineEnd = db.tellg();


                // 如果 cache 沒滿，就直接將 <key, lineEnd> insert 進 cache 以及 victim pool
                if ( KeyValueStorage::lineEndCache[dbID].size() < MaxCacheSize ) {
                    KeyValueStorage::lineEndCache[dbID].insert( make_pair( key, lineEnd ) );
                    KeyValueStorage::victimPool[dbID].insert( key );
                }

                // 如果 cache 滿了，隨機選一個 victim 替換掉
                else {

                    // 如果 victim 沒東西，重設所有 victim
                    if ( KeyValueStorage::victimPool[dbID].empty() == true )
                        KeyValueStorage::victimPool[dbID].swap( KeyValueStorage::usedPool[dbID] );

                    // 隨機選一個 victim
                    auto itVictim = KeyValueStorage::victimPool[dbID].begin();
                    std::advance( itVictim, random() % KeyValueStorage::victimPool[dbID].size() );

                    // 從 cache 以及 victim pool 刪除 victim
                    KeyValueStorage::victimPool[dbID].erase( itVictim );
                    KeyValueStorage::lineEndCache[dbID].erase( key );

                    // 插入 <key, lineEnd> 到 cache 以及 victim pool
                    KeyValueStorage::victimPool[dbID].insert( key );
                    KeyValueStorage::lineEndCache[dbID].insert( make_pair( key, lineEnd ) );
                }

                return make_pair( lineEnd, tmp );
            }
        }

        return make_pair( -1, "EMPTY" );
    }
}


void KeyValueStorage::UpdateDBFromPutBuffer( map<string, string> &mPutBuf, int dbID ) {

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
        long lineEnd = KeyValueStorage::FindKeyLineEndFrom( reader, pKV.first, dbID ).first;

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
    auto result = KeyValueStorage::FindKeyLineEndFrom( reader, key, dbID );
    reader.clear();
    reader.close();

    // lineEnd == -1 代表找不到 key，回傳 "EMPTY"
    if ( result.first == -1 )
        return "EMPTY";

    // 如果有找到 key 就抓 keyline(res.second) 最後 128 個 char(不含\n)
    int len = result.second.length();
    return result.second.substr( len - 128, 128 );
}
