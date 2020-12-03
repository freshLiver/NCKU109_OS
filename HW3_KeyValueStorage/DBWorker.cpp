#include "DBWorker.h"


/***********************************************
**************** public methods ****************
***********************************************/

std::mutex DBWorker::DBWorker::writeLock;

DBWorker::DBWorker( int id ) {

    // 設定 db id
    this->myDBID = id;

    // 根據 id 決定 db 路徑
    this->myDBPath = "./db/db0";
    this->myDBPath[this->myDBPath.length() - 1] = '0' + id;
}


DBWorker::~DBWorker() {}


string DBWorker::GetValueByKey( string key ) {

    // 開啟並從自己的 db 中搜尋這個 key
    fstream reader( this->myDBPath.c_str(), std::ios_base::in );

    auto result = DBWorker::FindKeyLineEndFrom( reader, key );

    // ! 關閉 db
    reader.close();

    // lineEnd == -1 代表找不到 key，回傳 "EMPTY"
    if ( std::get<0>( result ) == -1 )
        return "EMPTY";

    // 如果有找到 key 就解析 keyline、獲得 value 後回傳
    return GetValueFromKeyLine( std::get<1>( result ) );
}


void DBWorker::UpdateKeyValue( string key, string value ) {

    // read mode open my db
    fstream reader( this->myDBPath.c_str(), std::ios_base::in );

    // 從 db 裡搜尋 key
    auto result = DBWorker::FindKeyLineEndFrom( reader, key );

    // ! 搜尋完就關閉 fs
    reader.close();

    // ! key 在 db 中 -> Replace 原本的 key
    if ( std::get<0>( result ) != -1 ) {
        // ! write file
        this->myDB = fopen( this->myDBPath.c_str(), "r+" );

        // write pointer 移動到 line end - 129(包含 \n) 的位置
        fseek( this->myDB, std::get<0>( result ) - 129, SEEK_SET );

        // 用新 value 取代原本的 value 並且 flush
        DBWorker::writeLock.lock();
        fputs( value.c_str(), this->myDB );
        fflush( this->myDB );
        DBWorker::writeLock.unlock();

        // ! 結束後關閉 my db
        fclose( this->myDB );
    }

    // ! key 不在 db 中 -> echo append new value
    else {
        // echo "key[<=20] value[128]\n" >> ./db/dbx
        this->myDB = fopen( this->myDBPath.c_str(), "a+" );

        DBWorker::writeLock.lock();
        fputs( key.c_str(), this->myDB );
        fputs( " ", this->myDB );
        fputs( value.c_str(), this->myDB );
        fputs( "\n", this->myDB );
        fflush( this->myDB );
        DBWorker::writeLock.unlock();

        fclose( this->myDB );
    }
    return;
}


/************************************************
**************** private methods ****************
************************************************/


tuple<long, string> DBWorker::FindKeyLineEndFrom( fstream &db, string key ) {

    // 清除 eof flag 並回到檔案開頭
    db.seekg( 0, std::ios::beg );

    // 依據查詢各行
    for ( string tmp; getline( db, tmp ); ) {

        // 比對該行 key
        bool sameKey = true;
        for ( int i = 0; ( key[i] != '\0' ) && sameKey; ++i )
            sameKey = ( key[i] == tmp[i] );

        // 若 key 完全相同就回傳結果
        if ( sameKey )
            return std::make_tuple( db.tellg(), tmp );
    }

    return std::make_tuple( -1, "EMPTY" );
}


string DBWorker::GetValueFromKeyLine( string keyLine ) {

    // 最後 128 個 char(不含\n) 都是 value
    string value( 128, '\0' );

    for ( int iValue = 0, iCmd = ( keyLine.length() - 128 ); iValue < 128; ++iValue, ++iCmd )
        value[iValue] = keyLine[iCmd];

    return value;
}
