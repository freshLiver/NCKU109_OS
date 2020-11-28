#include "DBWorker.h"

DBWorker::DBWorker( int id ) {
    this->myDBID = id;
    this->myDBPath = "./db/db0";
    this->myDBPath[this->myDBPath.length() - 1] = '0' + id;
}

string DBWorker::GetValueByKey( string key ) {
    //
}

void DBWorker::UpdateKeyValue( string key, string value ) {
    //
}
