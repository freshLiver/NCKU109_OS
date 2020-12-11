#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include <map>
#include <iterator>
#include <set>

using namespace std;

map<string, long> myCahce;
set<string> no_used_keys;
set<string> used_keys;

pair<string, long> get_line_end_of( string key );

string get_and_push_to_cache( string key ) {

    // check if this key in db
    iterator itCache = myCahce.find( key );

    string lineCmd = itCache->first;
    long lineEnd = itCache->second;

    // this key in db
    if ( lineEnd != -1 ) {

        // try to push to used_keys if in unused
        iterator itKey = no_used_keys.find( key );

        if ( itKey != no_used_keys.end() ) {
            used_keys.insert( key );
            no_used_keys.erase( itKey );
        }

        // return last 128 chars as value
        return lineCmd.substr( lineCmd.length() - 128, lineCmd.length() );
    }

    // this key not in db
    else {
        // try to find key in db
        tie( lineCmd, lineEnd ) = get_line_end_of( key );

        // NOT IN DB
        if ( lineEnd == -1 ) {
            return "EMPTY";
        }
        // if this key in db (cache fault)
        else {
            // push to no used set
            no_used_keys.insert( key );

            // try to push to my cache (LIMITED SIZE)
            if ( myCahce.size() < 10000 )
                myCahce.insert( pair( key, lineEnd ) );

            // if cache is FULL
            else {
                // if no used is empty, swap used and no used
                no_used_keys.swap( used_keys );

                // random select one victim
                auto vimtim = no_used_keys.begin();
                std::advance( victim, random() % no_used_keys.size() );
                myCahce.erase( *victim );
                myCahce.insert( pair( key, lineEnd ) );
            }

            // return last 128 chars as value
            return lineCmd.substr( lineCmd.length() - 128, lineCmd.length() );
        }
    }
}


int main( int argc, char const *argv[] ) {
    /* code */
    return 0;
}
