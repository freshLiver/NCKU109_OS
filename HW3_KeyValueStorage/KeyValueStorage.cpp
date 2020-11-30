#include "KeyValueStorage.h"

using std::get;
using std::make_tuple;

/***********************************************
**************** public methods ****************
***********************************************/


KeyValueStorage::KeyValueStorage( string input, string output ) {

    // get input and output file name
    KeyValueStorage::inputFile = input;
    KeyValueStorage::outputFile = output;

    // 檢查是否所有 db 檔案都存在，若不存在就需要建立檔案
    system( "mkdir ./db/" );
    for ( int dbID = 0; dbID < 10; ++dbID ) {
        string touchCmd = "touch ./db/db0";
        touchCmd[touchCmd.length() - 1] = '0' + dbID;
        system( touchCmd.c_str() );
    }

    // 開啟 input file
    fstream fin( input.c_str() );

    // 建立 size N 的 cmd buffer, 一次讀 N 行 直到 eof
    int bufSize = 10000, readCmds;
    string *cmdBuffer = new string[bufSize];

    do {
        // get N commands
        readCmds = ReadNCommands( fin, bufSize, cmdBuffer );

        // 依序 quick parse 各個 cmd 並推給 workerThread 進階分析
        int index;
        bool isPUT = true;
        CmdType type;
        string key, value;

        for ( int thID = 0; thID < 10; ++thID )
            KeyValueStorage::workerThreads[thID] = thread( ParseUntilNotPUT, isPUT, thID );

        for ( int iBuffer = 0; iBuffer < readCmds; ++iBuffer ) {
            std::tie( type, index ) = QuickParseCmd( cmdBuffer[iBuffer] );

            // 若是 PUT 就直接加入 worker 的 queue
            if ( type == PUT )
                KeyValueStorage::workerTODO->push( cmdBuffer[iBuffer] );

            // 如果是 GET 或是 SCAN 則需要等待資料都更新完才能作業
            else {
                // 設定 pause flag 並等待所有 thread（worker）結束
                isPUT = false;
                for ( int i = 0; i < 10; ++i )
                    KeyValueStorage::workerThreads[i].join();


                // SCAN 跟 GET 只差在在 SCAN 需要進行多次 GET, 兩種都是從 key1 開始 GET
                string key1;
                LL numOfKeys, beginKey;

                if ( type == GET ) {
                    KeyValueStorage::GetKeyFromCmd( cmdBuffer[iBuffer], key1, 4, '\0' );
                    beginKey = stoll( key1 );
                    numOfKeys = 1;
                }
                else {
                    string key2;
                    std::tie( key1, key2 ) = KeyValueStorage::ParseCommandAs( SCAN, cmdBuffer[iBuffer] );
                    beginKey = stoll( key1 );
                    numOfKeys = stoll( key2 ) - stoll( key1 ) + 1;
                }

                // 開啟所有 worker
                vector<DBWorker *> workers;
                for ( int id = 0; id < 10; ++id )
                    workers.push_back( new DBWorker( id ) );

                // 根據 numOfKeys 從 beginKey 開始進行 GET
                for ( LL i = 0; i < numOfKeys; ++i, ++beginKey ) {

                    // quick parse 的時候就知道 index 了
                    string value = workers[index]->GetValueByKey( std::to_string( beginKey ) );

                    // print value(GET result)
                    DEBUG( "%lld is %s", beginKey, value.c_str() );
                }

                // ! 關閉所有 worker
                for ( int id = 0; id < 10; ++id )
                    delete workers[id];

                // 所有 thread 都結束後重設 flag 並重新啟動 thread
                isPUT = true;
                for ( int thID = 0; thID < 10; ++thID )
                    KeyValueStorage::workerThreads[thID] = thread( ParseUntilNotPUT, isPUT, thID );
            }
        }
    } while ( readCmds == -1 );
}



/************************************************
**************** private methods ****************
************************************************/


void KeyValueStorage::ParseUntilNotPUT( bool &isPUT, int thID ) {
    // init worker and get its todo queue
    DBWorker *worker = new DBWorker( thID );
    auto myQueue = KeyValueStorage::workerTODO[thID];

    // do todo tasks until not PUT cmd
    while ( isPUT || !myQueue.empty() ) {

        // pop front cmd
        string cmd = myQueue.front();
        myQueue.pop();

        // parse this cmd
        string key, value;
        std::tie( key, value ) = KeyValueStorage::ParseCommandAs( PUT, cmd );

        // worker update this pair
        worker->UpdateKeyValue( key, value );
    }

    // NOT PUT and all task done, delete worker
    delete worker;
    return;
}


int KeyValueStorage::ReadNCommands( fstream &fin, int maxLines, string cmdBuffer[] ) {
    // 一次讀最多 maxLines lines 到 buffer 中
    int count;
    for ( count = 0; count < maxLines; ++count )
        std::getline( fin, cmdBuffer[count] );

    // 檢查 eof, 如果 eof 就回傳 count 數量
    return fin.eof() ? count : -1;
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


tuple<string, string> KeyValueStorage::ParseCommandAs( CmdType type, string cmd ) {

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
