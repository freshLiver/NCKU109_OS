#include <bits/stdc++.h>

using namespace std;

pair<string, string> Cmd2KeyValuePair( string cmd ) {
    char *dmp = strdup( cmd.c_str() );

    string tmp, key, value;
    tmp = string( strtok( dmp, " " ) );
    key = string( strtok( NULL, " " ) );
    value = string( strtok( NULL, " " ) );

    free( dmp );
    return make_pair( key, value );
}

long GetLineEnd( string &output, string key, string value ) {
    // open fs as io mode
    long lineEnd = -1;
    fstream fout( output.c_str() );

    // search from db
    for ( string tmp; getline( fout, tmp ); ) {
        // dmp this line
        char *dmp = strdup( tmp.c_str() );
        // get key
        char *k = strtok( dmp, " " );
        // compare key
        if ( strcmp( k, key.c_str() ) == 0 )
            lineEnd = fout.tellg();
    }

    fout.close();
    return lineEnd;
}

void NoMapOutput( string &input, string &output ) {
    //
    fstream fin( input.c_str(), ios_base::in );

    // input all cmds to buffer
    queue<pair<string, string>> cmdBuffer;

    for ( string cmd; getline( fin, cmd ); )
        // convert to key value pair and output to buffer
        cmdBuffer.push( Cmd2KeyValuePair( cmd ) );


    // output cmds to file with out
    for ( pair<string, string> kv; !cmdBuffer.empty(); cmdBuffer.pop() ) {
        kv = cmdBuffer.front();
        // try to find key in output file
        long lineEnd = GetLineEnd( output, kv.first, kv.second );

        // if -1, app
        if ( lineEnd == -1 ) {
            fstream fout( output.c_str(), ios_base::app );
            fout << kv.first + " " + kv.second << endl;
            fout.flush();
            fout.close();
        }

        // if not -1, overwrite
        else {
            fstream fout( output.c_str() );
            fout.seekp( lineEnd - 129 );
            fout << kv.second << endl;
            fout.flush();
            fout.close();
        }
    }

    return;
}

void MapOutput( string &input, string &output ) {
    //
    fstream fin( input.c_str(), ios_base::in );

    // input all cmds to buffer
    queue<pair<string, string>> cmdBuffer;
    map<string, string> buffer;

    for ( string cmd; getline( fin, cmd ); )
        // convert to key value pair and output to buffer
        cmdBuffer.push( Cmd2KeyValuePair( cmd ) );

    // use map buffer
    for ( pair<string, string> kv; !cmdBuffer.empty(); cmdBuffer.pop() ) {
        // get front
        kv = cmdBuffer.front();

        // insert or update
        auto itBuf = buffer.find( kv.first );
        if ( itBuf != buffer.end() )
            itBuf->second = kv.second;
        else
            buffer.insert( kv );
    }

    // output buffer
    for ( auto &kv : buffer ) {
        // try to find key in output file
        long lineEnd = GetLineEnd( output, kv.first, kv.second );

        // if -1, app
        if ( lineEnd == -1 ) {
            fstream fout( output.c_str(), ios_base::app );
            fout << kv.first + " " + kv.second << endl;
            fout.flush();
            fout.close();
        }

        // if not -1, overwrite
        else {
            fstream fout( output.c_str() );
            fout.seekp( lineEnd - 129 );
            fout << kv.second << endl;
            fout.flush();
            fout.close();
        }
    }

    return;
}

int main( int argc, char const *argv[] ) {

    string input = "./10K_PUTs_from_10.input";
    string output = "./output.txt";
    system( ( "touch " + output ).c_str() );

    MapOutput( input, output );

    return 0;
}
