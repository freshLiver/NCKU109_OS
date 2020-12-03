#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>

using namespace std;


void fs_this_file( const char *file ) {
    fstream fout( file, ios_base::app );
    for ( int i = 0; i < 100; ++i ) {
        string str = to_string( rand() % INT32_MAX );
        fout << str;
        fout.flush();
    }
    fout.close();
}

void echo_this_file( const char *file ) {
    string str( 20, '0' );
    str[10] = '\0';

    char cmd[40];
    sprintf( cmd, "echo \"%s\" >> %s", str.c_str(), "./echo.out.test" );

    system( cmd );
}

static void flush_in_another_thread( string file, fstream *fout ) {
    //
    fout->write( "EEEE", 4 );
}

int main( int argc, char const *argv[] ) {
    string file1 = "./test.txt";
    string file2 = "./test2.txt";
    fstream fs1( file1.c_str(), ios_base::app );
    fstream fs2( file2.c_str(), ios_base::app );

    thread th1( flush_in_another_thread, file1, &fs1 );
    thread th2( flush_in_another_thread, file2, &fs2 );
    th1.join();
    th2.join();

    fs1.flush();
    fs2.flush();

    fs1.close();
    fs2.close();

    return 0;
}
