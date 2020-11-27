#if !defined( DBIO_h )
#define DBIO_h

#include <fstream>
#include <string>
#include <tuple>

using std::fstream;
using std::string;
using std::tuple;


class FileIO {
  public:
    /**
     *
     *
     * @brief 從某個以開啟的檔案中尋找特定 key 值，並且回傳搜尋結果以及該行位置
     *
     * @param key   目標 key 值
     * @param db    想要搜尋 key 值的檔案的 fstream 物件
     *
     * @return tuple<bool, long>, 是否找到 key, key 的行結尾位置（含 \\n）
     */
    static tuple<bool, long> FindKeyLineEndFromDB( string key, fstream &db ) {
        // 回到檔案開頭
        db.seekp( 0 );
        for ( string tmp; getline( db, tmp ); ) {
            // 比對該行 key
            bool sameKey = true;
            for ( int i = 0; i < key.size() && sameKey; ++i )
                sameKey = ( key[i] == tmp[i] );

            // 若 key 完全相同就回傳結果
            if ( sameKey )
                return std::make_tuple( true, db.tellp() );
        }
        return std::make_tuple( false, -1 );
    }
    /**
     *
     *
     * @brief 更新 key 對應的 value
     *
     * @param db            想要搜尋 key 值的檔案的 fstream 物件
     * @param keyLineEnd    目標 key 所在的行尾位置
     * @param newValue      新的 value
     */
    static void ReplaceValue( fstream &db, long keyLineEnd, string newValue ) {
        // 行尾有 \n, lineEndPos 在 \n 之後, 所以需要 -129 回到 value 的第一個 char
        db.seekp( keyLineEnd - 129 );
        // 只更新 value 的 128 個 char, 不更新 \n
        db.write( newValue.c_str(), 128 );
    }
    /**
     *
     *
     * @brief 一次從 target 讀取 N 行 string, 若是遇到 EOF 則會提早結束
     *
     * @param target    目標 file stream 的參考
     * @param N         一次要讀幾行
     * @param buf       要用來儲存 getline 結果的 buffer, size 應大於等於 N
     *
     * @return int : 若遇到 EOF 會回傳讀取行數, 否則回傳 -1 (讀了 N 行)
     */
    static int ReadNLines( fstream &target, int N, string buf[] ) {
        int count;
        for ( count = 0; count < N && std::getline( target, buf[count] ); ++count )
            ;
        //
        return target.eof() ? count : -1;
    }
};

#endif        // FileIO_h