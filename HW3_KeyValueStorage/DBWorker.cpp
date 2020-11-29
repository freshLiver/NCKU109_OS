#include "DBWorker.h"

/**
 * @brief Construct a new DBWorker::DBWorker object
 *
 * @param id
 */
DBWorker::DBWorker( int id ) {
    // 設定 db id
    this->myDBID = id;

    // 根據 id 決定 db 路徑
    this->myDBPath = "./db/db0";
    this->myDBPath[this->myDBPath.length() - 1] = '0' + id;

    // 開啟 db
    this->myDB = fstream( myDBPath );
}


/**
 * @brief
 *
 * @param key
 * @return string
 */
string DBWorker::GetValueByKey( string key ) {
    // 從自己的 db 中搜尋這個 key
    auto result = DBWorker::FindKeyLineEndFrom( this->myDB, key );

    // lineEnd == -1 代表找不到 key，回傳 "EMPTY"
    if ( std::get<0>( result ) == -1 )
        return "EMPTY";

    // 如果有找到 key 就解析 keyline、獲得 value 後回傳
    return GetValueFromKeyLine( std::get<1>( result ) );
}


/**
 * @brief
 *
 * @param key
 * @param value
 */
void DBWorker::UpdateKeyValue( string key, string value ) {
    // 從 db 裡搜尋 key
    auto result = DBWorker::FindKeyLineEndFrom( this->myDB, key );

    // ! key 在 db 中 -> Replace 原本的 key
    if ( std::get<0>( result ) != -1 ) {
        // write pointer 移動到 line end - 129(包含 \n) 的位置
        this->myDB.seekp( std::get<0>( result ) - 129 );

        // 用新 value 取代原本的 value
        this->myDB.write( value.c_str(), 128 );
    }
    // ! key 不在 db 中 -> Insert new Value
    else {
        // re-open db and change to append mode
        this->myDB.close();
        this->myDB = fstream( this->myDBPath.c_str(), std::ios_base::app );

        // append key and space
        this->myDB.write( key.c_str(), key.length() );
        this->myDB.write( " ", 1 );

        // append value and newline
        this->myDB.write( value.c_str(), 128 );
        this->myDB.write( "\n", 1 );

        // re-open db and change to default io mode
        this->myDB.close();
        this->myDB = fstream( this->myDBPath.c_str() );
    }
}
