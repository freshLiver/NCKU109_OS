#if !defined( KEYVALUESTORAGE_H )
#define KEYVALUESTORAGE_H

#include "macros.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <unistd.h>

using std::fstream;
using std::iterator;
using std::map;
using std::pair;
using std::queue;
using std::set;
using std::string;
using std::thread;

#define MaxBufSize 1000000
#define MaxCacheSize 100000
#define ThreadingThreshold 10000
#define DBNum 10

class KeyValueStorage {

public:
private:
    static fstream fin, fout;
    static string *cmdBuffer;
    static map<string, long> *lineEndCache;
    static set<string> *usedPool;
    static set<string> *victimPool;

public:
    /**
     * @brief 根據 input 以及 output file 進行初始化並處理資料庫
     *
     * @param input input 檔案路徑
     * @param output output 檔案路徑
     */
    KeyValueStorage( string &input, string &output );

    static void ParseTodoBuffer( queue<string> &qTodoBuf, map<string, string> &mPutBuf, int dbID );

    static void UpdateDBFromPutBuffer( map<string, string> &mPutBuf, int dbID );

private:
    /**
     * @brief command 屬於哪類指令、應該分到哪個 index
     *
     * @param cmd command
     * @return tuple<CmdType, int> cmd 的 type 以及所屬 db index
     */
    static pair<CmdType, int> QuickParseCmd( string &cmd );


    /**
     * @brief 從 cmd[start] 開始抓 char 直到 ' '
     *
     * @param cmd command
     * @param key 抓出的 key 會在這，length 應為「20 chars」
     * @param start 從 cmd 的哪個 index 開始抓 key
     * @param delim 終止字符，用來判斷 key 的結尾（應為 ' ' 或是 '\0'）
     * @return int delim 的下一個 char 的 index，用於 SCAN 抓 key2
     */
    static int GetKeyFromCmd( string &cmd, string &key, int start, char delim ) {
        for ( int iCmd = start, iKey = 0;; ++iCmd, ++iKey ) {
            if ( cmd[iCmd] == delim )
                return ++iCmd;
            else
                key[iKey] = cmd[iCmd];
        }
    }


    /**
     * @brief 從 cmd[start] 開始抓 128 chars 的 value string
     *GetValueByKey
     * @param cmd command
     * @param value 128 chars 的 value 會儲存在這裡面，大小應為 128 chars
     * @param start 從哪個 index 開始抓 128 個 chars
     */
    static void GetValueFromCmd( string &cmd, string &value, int start ) {
        for ( int iCmd = start, iValue = 0; iValue < 128; ++iCmd, ++iValue )
            value[iValue] = cmd[iCmd];
    }


    /**
     * @brief 根據 commmand type parse 指令，抓出 command 的 key, value(or key2)
     *
     * @param type cmd 的類型
     * @param cmd command
     * @return tuple<string, string> command 的 key, value(or key2)
     */
    static pair<string, string> ParseCommandAs( CmdType type, string &cmd );


    static string GetValueByKey( string key, int dbID );


    static pair<long, string> FindKeyLineEndFrom( fstream &db, string key, int dbID );
};

#endif        // KEYVALUESTORAGE_H
