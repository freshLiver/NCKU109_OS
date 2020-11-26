#if !defined( FileIO_h )
#define FileIO_h

#include <fstream>
#include <string>
#include <tuple>

using std::string;
using std::fstream;
using std::tuple;


class FileIO {
  public:
    ///
    /// @brief
    /// 從某個以開啟的檔案中尋找特定 key 值，並且回傳搜尋結果以及該行位置
    /// @param  key 目標 key 值
    /// @param  db  想要搜尋 key 值的檔案的 fstream 物件
    /// @return     是否找到該 key, 發現 key 的該行結尾位置（含 \\n）
    ///
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
    ///
    /// @brief
    /// 更新 key 對應的 value
    /// @param  db          想要搜尋 key 值的檔案的 fstream 物件
    /// @param  keyLineEnd  目標 key 所在的行尾位置
    /// @param  newValue    新的 value
    /// @return             None
    ///
    static void ReplaceValue( fstream &db, long keyLineEnd, string newValue ) {
        // 行尾有 \n, lineEndPos 在 \n 之後, 所以需要 -129 回到 value 的第一個 char
        db.seekp( keyLineEnd - 129 );
        // 只更新 value 的 128 個 char, 不更新 \n
        db.write( newValue.c_str(), 128 );
    }
    ///
    /// @brief
    ///
    /// @param
    /// @param
    /// @return
    ///
};

#endif        // FileIO_h