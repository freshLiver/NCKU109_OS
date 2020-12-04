#include "KeyValueStorage.h"

using std::get;
using std::make_tuple;

/***********************************************
**************** public methods ****************
***********************************************/


string KeyValueStorage::inputFile, KeyValueStorage::outputFile;
thread KeyValueStorage::workerThreads[10];
queue<string> KeyValueStorage::workerTODO[10];
std::mutex KeyValueStorage::workerLock[10];

KeyValueStorage::KeyValueStorage( string &input, string &output ) {

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

    // ! pass pointer, do not recycle in every round
    bool isPUT = true;

    do {
        // get N commands
        readCmds = ReadNCommands( &fin, bufSize, cmdBuffer );

        // ? 測試 read 到的 N 行
        for ( int i = 0; i < readCmds; ++i ) {
            string cmd = "echo \"" + cmdBuffer[i] + "\" >> cmds.output";
            system( cmd.c_str() );
        }

        // 依序 quick parse 各個 cmd 並推給 workerThread 進階分析
        int index;
        CmdType type;

        // 啟動所有 thread workers
        isPUT = true;
        for ( int thID = 0; thID < 10; ++thID )
            KeyValueStorage::workerThreads[thID] = thread( KeyValueStorage::ParseUntilNotPUT, &isPUT, thID );

        // 處理所有 buffer 中的 command
        for ( int iBuffer = 0; iBuffer < readCmds; ++iBuffer ) {
            std::tie( type, index ) = QuickParseCmd( cmdBuffer[iBuffer] );

            // 若是 PUT 就直接加入 worker 的 queue
            if ( type == PUT ) {
                KeyValueStorage::workerLock[index].lock();
                KeyValueStorage::workerTODO[index].push( cmdBuffer[iBuffer] );
                KeyValueStorage::workerLock[index].unlock();
            }

            // 如果是 GET 或是 SCAN 則需要等待資料都更新完才能作業
            else {
                // 設定 pause flag 並等待所有 thread（worker）結束
                isPUT = false;
                for ( int i = 0; i < 10; ++i )
                    KeyValueStorage::workerThreads[i].join();


                // SCAN 跟 GET 只差在在 SCAN 需要進行多次 GET, 兩種都是從 key1 開始 GET
                string key1( 20, '\0' );
                LL numOfKeys, beginKey;

                if ( type == GET ) {
                    KeyValueStorage::GetKeyFromCmd( cmdBuffer[iBuffer], key1, 4, '\0' );
                    beginKey = stoll( key1 );
                    numOfKeys = 1;
                }
                else {
                    string key2( 20, '\0' );
                    std::tie( key1, key2 ) = KeyValueStorage::ParseCommandAs( SCAN, cmdBuffer[iBuffer] );
                    beginKey = stoll( key1 );
                    numOfKeys = stoll( key2 ) - stoll( key1 ) + 1;
                }


                // 開啟所有 worker
                vector<DBWorker *> workers;
                for ( int id = 0; id < 10; ++id )
                    workers.push_back( new DBWorker( id ) );

                // 根據 numOfKeys 從 beginKey 開始進行 GET 並輸出到 output 檔案中
                fstream oStream( output.c_str(), std::ios_base::app );
                for ( LL i = 0; i < numOfKeys; ++i, ++beginKey ) {
                    // GET value of this key
                    string value = workers[index]->GetValueByKey( std::to_string( beginKey ) );

                    // output GET result(value) and newline
                    oStream.write( value.c_str(), value.length() );
                    oStream.write( "\n", 1 );
                }

                // flush and close output stream
                oStream.flush();
                oStream.close();

                // ! 關閉所有 worker
                for ( int id = 0; id < 10; ++id )
                    delete workers[id];

                // 所有 thread 都結束後重設 flag 並重新啟動 thread
                isPUT = true;
                for ( int thID = 0; thID < 10; ++thID )
                    KeyValueStorage::workerThreads[thID] = thread( KeyValueStorage::ParseUntilNotPUT, &isPUT, thID );
            }
        }

        // ! 要先等待所有 worker 結束才能進入下一輪
        isPUT = false;
        for ( int i = 0; i < 10; ++i )
            KeyValueStorage::workerThreads[i].join();

    } while ( readCmds == -1 );
}



/************************************************
**************** private methods ****************
************************************************/


void KeyValueStorage::ParseUntilNotPUT( bool *isPUT, int thID ) {
    // init worker and get its todo queue
    DBWorker *worker = new DBWorker( thID );
    auto myQueue = &( KeyValueStorage::workerTODO[thID] );

    // do todo tasks until not PUT cmd
    string cmd, key, value;
    try {
        while ( *isPUT == true ) {
            // check queue state
            while ( !myQueue->empty() ) {
                // pop front cmd
                KeyValueStorage::workerLock[thID].lock();
                cmd = myQueue->front();
                myQueue->pop();
                KeyValueStorage::workerLock[thID].unlock();

                // parse PUT cmd and update db
                std::tie( key, value ) = KeyValueStorage::ParseCommandAs( PUT, cmd );
                worker->UpdateKeyValue( key, value );
            }
        }
    }
    catch ( std::exception &e ) {
        DEBUG( "at thread %d, isPUT: %d, empty: %d, \ncmd :%s, \nfront: %s",
               thID,
               *isPUT,
               myQueue->empty(),
               cmd.c_str(),
               myQueue->front().c_str() );

        DEBUG( "what() :%s", e.what() );

        // ? print current todo queue
        for ( int i = 0; i < myQueue->size(); ++i ) {
            DEBUG( "queue : %s\n", myQueue->front().c_str() );
            myQueue->pop();
        }
    }

    // NOT PUT and all task done, delete worker
    delete worker;
    return;
}


int KeyValueStorage::ReadNCommands( fstream *fin, int maxLines, string *cmdBuffer ) {
    // 一次讀最多 maxLines lines 到 buffer 中
    int count;
    for ( count = 0; count < maxLines && !fin->eof(); ++count )
        std::getline( *fin, cmdBuffer[count] );

    // 檢查 eof, 如果 eof 就回傳 count 數量
    return fin->eof() ? count : maxLines;
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
