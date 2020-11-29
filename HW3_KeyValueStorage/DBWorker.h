#if !defined( DBWORKER_h )
#define DBWORKER_h

#include <cstdlib>
#include <fstream>
#include <queue>
#include <string>
#include <tuple>

using std::fstream;
using std::string;
using std::queue;
using std::tuple;

class DBWorker {

private:
    fstream myDB;
    int myDBID;
    string myDBPath;
    // queue<string, string> putsBuffer;

public:


    /**
     * @brief 建立 DBWorker 並根據 id 分配 db
     *
     * @param id Worker 負責的 db 編號（0~9）
     */
    DBWorker( int id );


    /**
     * @brief 從此 worker 負責的 db 中根據 key 取出 value
     *
     * @param key 目標 key 值
     * @return string key 對應的 value，找不到 key 則回傳 "EMPTY"
     */
    string GetValueByKey( string key );


    /**
     * @brief 從此 worker 負責的 db 中更新 key value pair
     *
     * @param key 目標 key 值
     * @param value 要更新或是新增的值，應由 128 位 [A-Za-z0-9] 組成
     */
    void UpdateKeyValue( string key, string value );



private:


    /**
     *
     * @brief 從某個以開啟的檔案中尋找特定 key 值
     *
     * @param key   目標 key 值
     * @param db    想要搜尋 key 值的檔案的 fstream 物件
     *
     * @return tuple<long, string> key 的列結尾位址(找不到就回傳 -1), key 整列字串
     */
    static tuple<long, string> FindKeyLineEndFrom( fstream &db, string key ) {
        // 清除 eof flag 並回到檔案開頭
        db.clear();
        db.seekg( 0, std::ios::beg );

        // 依據查詢各行
        for ( string tmp; getline( db, tmp ); ) {
            // 比對該行 key
            bool sameKey = true;
            for ( int i = 0; i < key.size() && sameKey; ++i )
                sameKey = ( key[i] == tmp[i] );

            // 若 key 完全相同就回傳結果
            if ( sameKey )
                return std::make_tuple( db.tellp(), tmp );
        }
        return std::make_tuple( -1, "EMPTY" );
    }


    /**
     * @brief
     *
     * @param keyLine
     * @return string
     */
    static string GetValueFromKeyLine( string keyLine ) {
        // 最後 128 個 char(不含\n) 都是 value
        string value( 128, '\0' );
        for ( int iValue = 0, iCmd = ( keyLine.length() - 129 ); iValue < 128; ++iValue, ++iCmd )
            value[iValue] = keyLine[iCmd];
        return value;
    }
};

#endif        // DBWORKER_h