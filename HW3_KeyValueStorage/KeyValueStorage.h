#if !defined( KEYVALUESTORAGE_H )
#define KEYVALUESTORAGE_H

#include "macros.h"
#include <cstdio>
#include <fstream>
#include <map>
#include <queue>
#include <string>

using std::fstream;
using std::queue;
using std::string;
using std::map;
using std::pair;

#define MaxBufSize 10000
#define DBNum 10

class KeyValueStorage {

  public:
  private:
    static string inputFile, outputFile;
    static string *cmdBuffer;

  public:
    /**
     * @brief 根據 input 以及 output file 進行初始化並處理資料庫
     *
     * @param input input 檔案路徑
     * @param output output 檔案路徑
     */
    KeyValueStorage( string &input, string &output );

  private:
    static void ParseTodoBuffer( queue<string> &qTodoBuf, map<string, string> &mPutBuf, int &dbID );

    /**
     * @brief 一次讀取最多 N lines command 並存在 buffer 中，直到 eof
     *
     * @param fin       input file fstream
     * @param maxLine   最大讀取 line 數
     * @param cmdBuffer 讀取的 cmd 得暫存器，大小應比 maxLine 大
     * @return int      eof 時讀取了幾行，非 eof 時必為 maxLine
     */
    static int ReadNCommands( fstream &fin, int maxLine, string cmdBuffer[] );


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
    static int GetKeyFromCmd( string &cmd, string &key, int start, char delim );


    /**
     * @brief 從 cmd[start] 開始抓 128 chars 的 value string
     *
     * @param cmd command
     * @param value 128 chars 的 value 會儲存在這裡面，大小應為 128 chars
     * @param start 從哪個 index 開始抓 128 個 chars
     */
    static void GetValueFromCmd( string &cmd, string &value, int start );


    /**
     * @brief 根據 commmand type parse 指令，抓出 command 的 key, value(or key2)
     *
     * @param type cmd 的類型
     * @param cmd command
     * @return tuple<string, string> command 的 key, value(or key2)
     */
    static pair<string, string> ParseCommandAs( CmdType type, string &cmd );
};

#endif        // KEYVALUESTORAGE_H