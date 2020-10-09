#include "multiqueue.h"
#include <cstdio>
#include <cstdlib>
#include <future>
#include <string>
#include <thread>
#include <unistd.h>


using std::pair;
using std::string;
using std::thread;
using std::future;
using std::async;

typedef unsigned int uint;
typedef long long LL;


static void SyncGet ( MultiQueue< int > *mqueue, uint target, LL *count ) {
    queue< int > q = mqueue->queues[target];
    while ( !q.empty ( ) ) {
        // printf ( "queue[%d] size : %ld\n", target, q.size ( ) );
        LL value = q.front ( );
        q.pop ( );
        ++( *count );
    }
    --mqueue->size;
}

int main ( int argc, char const *argv[] ) {

    const uint th_size = 4;
    MultiQueue< int > *mqueue = new MultiQueue< int > ( th_size );

    for ( int i = 0; i < 1000000; ++i )
        mqueue->push ( i );

    // threading get items
    LL count[th_size] = {0};
    future< void > fres[th_size];

    for ( int i = 0; i < th_size; ++i )
        fres[i] = async ( std::launch::async, SyncGet, mqueue, i, &count[i] );

    // result
    for ( int i = 0; i < th_size; ++i )
        fres[i].get ( );

    LL result = 0;
    for ( int i = 0; i < th_size; ++i )
        result += count[i];
    printf ( "final result count = %lld << \n", result );


    return 0;
}
