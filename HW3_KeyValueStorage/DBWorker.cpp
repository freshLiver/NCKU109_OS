#include "DBWorker.h"


/*******************************
******** public methods ********
*******************************/


DBWorker::DBWorker( int id ) {
    // 設定 db id
    this->myDBID = id;

    // 根據 id 決定 db 路徑
    this->myDBPath = "./db/db0";
    this->myDBPath[this->myDBPath.length() - 1] = '0' + id;

    // 開啟 db
    this->myDB = fstream( myDBPath );
}


string DBWorker::GetValueByKey( string key ) {
    // 從自己的 db 中搜尋這個 key
    auto result = DBWorker::FindKeyLineEndFrom( this->myDB, key );

    // lineEnd == -1 代表找不到 key，回傳 "EMPTY"
    if ( std::get<0>( result ) == -1 )
        return "EMPTY";

    // 如果有找到 key 就解析 keyline、獲得 value 後回傳
    return GetValueFromKeyLine( std::get<1>( result ) );
}


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


/********************************
******** private methods ********
********************************/


tuple<long, string> DBWorker::FindKeyLineEndFrom( fstream &db, string key ) {
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


string DBWorker::GetValueFromKeyLine( string keyLine ) {
    // 最後 128 個 char(不含\n) 都是 value
    string value( 128, '\0' );
    for ( int iValue = 0, iCmd = ( keyLine.length() - 129 ); iValue < 128; ++iValue, ++iCmd )
        value[iValue] = keyLine[iCmd];
    return value;
}
