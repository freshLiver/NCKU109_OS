#if !defined( DBWORKER_h )
#define DBWORKER_h

#include <cstdlib>
#include <fstream>
#include <queue>
#include <string>

using std::fstream;
using std::string;
using std::queue;

class DBWorker {
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
    fstream myDB;
    int myDBID;
    string myDBPath;
    // queue<string, string> putsBuffer;
};

#endif        // DBWORKER_h