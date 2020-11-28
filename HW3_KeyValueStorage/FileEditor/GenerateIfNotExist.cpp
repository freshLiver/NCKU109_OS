#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;

static void CreateFileIfNotExist( const char *path ) {
    FILE *target;
    if ( !( target = fopen( path, "r" ) ) )
        target = fopen( path, "w" );
    fclose( target );
    return;
}

static void CheckAllDB() {
    // 直接 mkdir
    system( "mkdir ./db/" );
    // 檢查所有 db 檔
    string target = "./db/db0";
    for ( int i = 0; i < 10; ++i ) {
        target[target.length() - 1] = '0' + i;
        CreateFileIfNotExist( target.c_str() );
    }
}

int main( int argc, char const *argv[] ) {
    CheckAllDB();
    return 0;
}
