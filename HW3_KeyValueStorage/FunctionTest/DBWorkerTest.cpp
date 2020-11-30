#include "../DBWorker.h"
#include <cstdlib>

void initDB() {
    system( "mkdir ./db/" );
    string touchCmd = "touch ./db/db0";
    for ( int i = 0; i < 10; ++i ) {
        touchCmd[touchCmd.length() - 1] = '0' + i;
        system( touchCmd.c_str() );
    }
}

int main( int argc, char const *argv[] ) {

    initDB();

    DBWorker worker( 0 );
    string value = worker.GetValueByKey( "2147480800" );
    printf( "get value :%s\n", value.c_str() );
    worker.UpdateKeyValue("2147480800","22EE88GGGWw8kYwlnlZYW2TErjTA2uqqLPlSKPOjOZaCkctdLeCee7ssnaBpN7yBNyN6a1Z9GQk0toKYBuC3xY99iAqHWndcBd4aV6ZXkbQEyJTfdpVKzzuoq4ZzHhkm");
    return 0;
}
