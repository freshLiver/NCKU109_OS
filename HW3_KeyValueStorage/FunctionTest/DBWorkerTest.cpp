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
    DBWorker worker0( 2 );

    string value = "EEEEXKRPNQEfqotp9RUNgRUbgeTQ9FKvvBgHWE5SWB2ko2Qz1DgGiVkMgmlOR3nHMVxE9OwMd0CHECRTaoLIoWElzSD2r0Jbcc1"
                   "8c4AZ0bbbjH3L6WbGbAiGsm9NbEIS";
    string value2 =
        "2222XKRPNQEfqotp9RUNgRUbgeTQ9FKvvBgHWE5SWB2ko2Qz1DgGiVkMgmlOR3nHMVxE9OwMd0CHECRTaoLIoWElzSD2r0Jbcc1"
        "8c4AZ0bbbjH3L6WbGbAiGsm9NbEIS";
    worker0.UpdateKeyValue( "2147482325", value );
    worker0.UpdateKeyValue( "777777777", value2 );

    // auto res0 = worker0.FindKeyLineEndFrom( db0, "2147483327" );
    auto res = worker0.GetValueByKey( "777777777" );
    res = worker0.GetValueByKey( "2147482325" );

    worker0.UpdateKeyValue( "2147482325", value2 );
    worker0.UpdateKeyValue( "777777777", value );

    return 0;
}
